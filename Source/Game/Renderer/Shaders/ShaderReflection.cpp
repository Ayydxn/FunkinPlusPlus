#include "FunkinPCH.h"
#include "ShaderReflection.h"

#include <slang-com-ptr.h>

namespace
{
    std::string ShaderResourceTypeToString(const EShaderResourceType Type)
    {
        switch (Type)
        {
            case EShaderResourceType::UniformBuffer: return "UniformBuffer";
            case EShaderResourceType::StorageBuffer: return "StorageBuffer";
            case EShaderResourceType::Sampler: return "Sampler";
            case EShaderResourceType::CombinedImageSampler: return "CombinedImageSampler";
            case EShaderResourceType::SampledImage: return "SampledImage";
            case EShaderResourceType::StorageImage: return "StorageImage";
            default: return "Unknown";
        }
    }

    std::string ShaderStageFlagsToString(const EShaderStage ShaderStageFlags)
    {
        std::string Result;

        if (HasShaderStageFlag(ShaderStageFlags, EShaderStage::Vertex))
            Result += "Vertex|";
        
        if (HasShaderStageFlag(ShaderStageFlags, EShaderStage::Fragment))
            Result += "Fragment|";
        
        if (HasShaderStageFlag(ShaderStageFlags, EShaderStage::Compute))
            Result += "Compute|";

        if (Result.empty())
            return "None";

        Result.pop_back(); // Trim the trailing '|'
        return Result;
    }
}

FShaderReflectionData CShaderReflection::Extract(slang::IComponentType* ComposedProgram, slang::ProgramLayout* ProgramLayout)
{
    verifyFunkinf(ProgramLayout, "Attempted to extract shader reflection data from a null ProgramLayout!")

    FShaderReflectionData ExtractedReflectionData;
    
    const uint32 GlobalParameterCount = ProgramLayout->getParameterCount();
    for (uint32 ParamIndex = 0; ParamIndex < GlobalParameterCount; ++ParamIndex)
    {
        slang::VariableLayoutReflection* Parameter = ProgramLayout->getParameterByIndex(ParamIndex);
        const EShaderStage UsedStageFlags = GetActualUsageStageFlags(ComposedProgram, ProgramLayout, Parameter);

        ExtractParameter(Parameter, UsedStageFlags, ExtractedReflectionData);
    }

    const SlangInt32 EntryPointCount = static_cast<SlangInt32>(ProgramLayout->getEntryPointCount());
    for (SlangInt32 EntryPointIndex = 0; EntryPointIndex < EntryPointCount; ++EntryPointIndex)
    {
        slang::EntryPointLayout* EntryPoint = ProgramLayout->getEntryPointByIndex(EntryPointIndex);
        const EShaderStage ShaderStageFlag = GetShaderStageFlagFromSlangStage(EntryPoint->getStage());

        const uint32 ParameterCount = EntryPoint->getParameterCount();
        for (uint32 ParamIndex = 0; ParamIndex < ParameterCount; ++ParamIndex)
        {
            slang::VariableLayoutReflection* Parameter = EntryPoint->getParameterByIndex(ParamIndex);
            ExtractParameter(Parameter, ShaderStageFlag, ExtractedReflectionData);
        }

        ExtractEntryPointPushConstants(EntryPoint, ShaderStageFlag, ExtractedReflectionData);
    }

    return ExtractedReflectionData;
}

void CShaderReflection::LogShaderReflectionData(const std::string& ShaderName, const FShaderReflectionData& ShaderReflectionData)
{
    LOG_INFO_TAG("Renderer", "Shader Reflection Data for '{}':", ShaderName);
    LOG_INFO_TAG("Renderer", "  Resources ({}):", ShaderReflectionData.Resources.size());

    for (const auto& [Name, Type, Binding, Set, ArraySize, StageFlags] : ShaderReflectionData.Resources)
    {
        LOG_INFO_TAG("Renderer", "    - '{}': Type={}, Set={}, Binding={}, ArraySize={}, Stages={}", Name, ShaderResourceTypeToString(Type), Set, Binding, ArraySize,
            ShaderStageFlagsToString(StageFlags));
    }

    LOG_INFO_TAG("Renderer", "  Push Constants ({}):", ShaderReflectionData.PushConstants.size());

    for (const auto& [Name, Size, Offset, StageFlags] : ShaderReflectionData.PushConstants)
        LOG_INFO_TAG("Renderer", "    - '{}': Offset={}, Size={}, Stages={}", Name, Offset, Size, ShaderStageFlagsToString(StageFlags));
}

void CShaderReflection::ExtractParameter(slang::VariableLayoutReflection* Parameter, EShaderStage StageFlags, FShaderReflectionData& OutReflectionData)
{
    slang::TypeLayoutReflection* TypeLayout = Parameter->getTypeLayout();
    if (!TypeLayout)
        return;

    // DescriptorTableSlot is Slang's backend-agnostic category for something that occupies a binding slot like a UBO, SSBO or texture.
    const bool bIsResourceBinding = TypeLayout->getParameterCategory() == slang::ParameterCategory::DescriptorTableSlot
        || TypeLayout->getParameterCategory() == slang::ParameterCategory::ShaderResource || TypeLayout->getParameterCategory() == slang::ParameterCategory::UnorderedAccess
        || TypeLayout->getParameterCategory() == slang::ParameterCategory::ConstantBuffer || TypeLayout->getParameterCategory() == slang::ParameterCategory::SamplerState;

    if (!bIsResourceBinding)
        return;

    const uint32_t Binding = Parameter->getBindingIndex();
    const uint32_t Set = Parameter->getBindingSpace();

    // The same resource (e.g. the camera UBO) is visited once per entry point that references it, since we walk the per-entry-point parameter lists.
    // Rather than pushing a duplicate FShaderReflectedResource per stage, find the existing entry for this set/binding and OR in the new stage.
    for (FShaderReflectedResource& ExistingResource : OutReflectionData.Resources)
    {
        if (ExistingResource.Set == Set && ExistingResource.Binding == Binding)
        {
            ExistingResource.StageFlags |= StageFlags;
            return;
        }
    }

    FShaderReflectedResource Resource;
    Resource.Name = Parameter->getName() ? Parameter->getName() : "<unnamed>";
    Resource.Type = GetResourceTypeFromCategory(TypeLayout);
    Resource.Binding = Binding;
    Resource.Set = Set;
    Resource.ArraySize = GetResourceArraySize(TypeLayout);
    Resource.StageFlags = StageFlags;

    OutReflectionData.Resources.push_back(Resource);
}

void CShaderReflection::ExtractEntryPointPushConstants(slang::EntryPointLayout* EntryPointLayout, EShaderStage StageFlag, FShaderReflectionData& OutReflectionData)
{
    slang::VariableLayoutReflection* EntryPointParams = EntryPointLayout->getVarLayout();
    if (!EntryPointParams || !EntryPointParams->getTypeLayout())
        return;

    slang::TypeLayoutReflection* ParamsTypeLayout = EntryPointParams->getTypeLayout();
    const unsigned int FieldCount = ParamsTypeLayout->getFieldCount();

    for (unsigned int i = 0; i < FieldCount; ++i)
    {
        slang::VariableLayoutReflection* Field = ParamsTypeLayout->getFieldByIndex(i);
        slang::TypeLayoutReflection* FieldTypeLayout = Field->getTypeLayout();

        if (!FieldTypeLayout || FieldTypeLayout->getParameterCategory() != slang::ParameterCategory::PushConstantBuffer)
            continue;

        FShaderReflectedPushConstant PushConstant;
        PushConstant.Name = Field->getName() ? Field->getName() : "Unnamed Push Constant";
        PushConstant.Offset = static_cast<uint32>(Field->getOffset(slang::ParameterCategory::PushConstantBuffer));
        PushConstant.StageFlags = StageFlag;

        // The push-constant block's size is the size of its element type layout (the struct behind the implicit constant buffer wrapper), not the wrapper itself.
        if (slang::TypeLayoutReflection* ElementTypeLayout = FieldTypeLayout->getElementTypeLayout())
            PushConstant.Size = static_cast<uint32>(ElementTypeLayout->getSize());

        OutReflectionData.PushConstants.push_back(PushConstant);
    }
}

EShaderStage CShaderReflection::GetActualUsageStageFlags(slang::IComponentType* ComposedProgram, slang::ProgramLayout* ProgramLayout,
    slang::VariableLayoutReflection* GlobalParameter)
{
    EShaderStage UsedStageFlags = EShaderStage::None;

    const auto Category = static_cast<SlangParameterCategory>(GlobalParameter->getCategory());
    const SlangInt32 EntryPointCount = static_cast<SlangInt32>(ProgramLayout->getEntryPointCount());

    for (SlangInt32 EntryPointIndex = 0; EntryPointIndex < EntryPointCount; ++EntryPointIndex)
    {
        slang::EntryPointLayout* EntryPoint = ProgramLayout->getEntryPointByIndex(EntryPointIndex);

        Slang::ComPtr<slang::IBlob> DiagnosticsBlob;
        Slang::ComPtr<slang::IMetadata> Metadata;
        
        const SlangResult Result = ComposedProgram->getEntryPointMetadata(EntryPointIndex, 0, Metadata.writeRef(), DiagnosticsBlob.writeRef());
        if (SLANG_FAILED(Result) || !Metadata)
            continue;

        bool bIsUsed = false;
        const SlangResult UsageQueryResult = Metadata->isParameterLocationUsed(Category, GlobalParameter->getBindingSpace(), GlobalParameter->getBindingIndex(),
            bIsUsed);

        if (SLANG_SUCCEEDED(UsageQueryResult) && bIsUsed)
            UsedStageFlags |= GetShaderStageFlagFromSlangStage(EntryPoint->getStage());
    }

    // If the metadata query failed outright for every entry point (rather than just reporting "unused"), fall back
    // to treating the resource as used by every stage instead of silently dropping it from all of them.
    if (UsedStageFlags == EShaderStage::None)
    {
        for (SlangInt32 EntryPointIndex = 0; EntryPointIndex < EntryPointCount; ++EntryPointIndex)
            UsedStageFlags |= GetShaderStageFlagFromSlangStage(ProgramLayout->getEntryPointByIndex(EntryPointIndex)->getStage());
    }

    return UsedStageFlags;
}

EShaderResourceType CShaderReflection::GetResourceTypeFromCategory(slang::TypeLayoutReflection* TypeLayout)
{
    // Arrays of resources report a kind of array at the top level with the actual resource kind living on the element type layout.
    // So, we unwrap any nesting (in case of multidimensional arrays) before inspecting the kind below.
    while (TypeLayout->getKind() == slang::TypeReflection::Kind::Array)
        TypeLayout = TypeLayout->getElementTypeLayout();
    
    switch (TypeLayout->getKind())
    {
        case slang::TypeReflection::Kind::ConstantBuffer: return EShaderResourceType::UniformBuffer;

        case slang::TypeReflection::Kind::Resource:
        {
            // Distinguish combined-image-samplers, structured/storage buffers, and separate textures/storage images by the resource's shape and access.
            slang::TypeReflection* Type = TypeLayout->getType();
            const SlangResourceShape FullShape = Type->getResourceShape();
            const SlangResourceShape BaseShape = static_cast<SlangResourceShape>(FullShape & SLANG_RESOURCE_BASE_SHAPE_MASK);
            const bool bIsCombined = (FullShape & SLANG_TEXTURE_COMBINED_FLAG) != 0;
            const SlangResourceAccess Access = Type->getResourceAccess();
 
            if (BaseShape == SLANG_STRUCTURED_BUFFER)
                return EShaderResourceType::StorageBuffer;
 
            // If Sampler2D/Sampler3D/etc. was used in the Slang source (which we expect since that's the standard)
            if (bIsCombined)
                return EShaderResourceType::CombinedImageSampler;
 
            if (Access == SLANG_RESOURCE_ACCESS_READ_WRITE)
                return EShaderResourceType::StorageImage;
 
            return EShaderResourceType::SampledImage;
        }
        
        case slang::TypeReflection::Kind::SamplerState: return EShaderResourceType::Sampler;

        default:
        {
            verifyFunkinf(false, "Encountered an unhandled Slang resource kind ({}) during shader reflection!", static_cast<int>(TypeLayout->getKind()));
            return EShaderResourceType::UniformBuffer;
        }
    }
}

uint32 CShaderReflection::GetResourceArraySize(slang::TypeLayoutReflection* TypeLayout)
{
    if (TypeLayout->getKind() == slang::TypeReflection::Kind::Array)
        return static_cast<uint32_t>(TypeLayout->getElementCount());
 
    return 1;
}

EShaderStage CShaderReflection::GetShaderStageFlagFromSlangStage(SlangStage Stage)
{
    switch (Stage)
    {
        case SLANG_STAGE_VERTEX: return EShaderStage::Vertex;
        case SLANG_STAGE_FRAGMENT: return EShaderStage::Fragment;
        case SLANG_STAGE_COMPUTE: return EShaderStage::Compute;

        default:
        {
            verifyFunkinf(false, "Encountered an unhandled Slang shader stage ({}) during shader reflection!",
                          static_cast<int32>(Stage))
            return EShaderStage::None;
        }
    }
}

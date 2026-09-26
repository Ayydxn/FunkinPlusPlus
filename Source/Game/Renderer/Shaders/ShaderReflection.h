#pragma once

#include "ShaderStages.h"
#include "Core/CoreTypes.h"

#include <string>
#include <vector>

#include <slang.h>

enum class EShaderResourceType : uint8
{
    UniformBuffer,
    StorageBuffer,
    Sampler,
    CombinedImageSampler,
    SampledImage,
    StorageImage
};

// One reflected resource binding (a UBO, SSBO, sampler, etc.) as declared explicitly in shader source via [[vk::binding(x, y)]].
// Reflection reads this back - it does not assign it.
struct FShaderReflectedResource
{
    std::string Name;
    EShaderResourceType Type = EShaderResourceType::UniformBuffer;
    uint32 Binding = 0;
    uint32 Set = 0;
    uint32_t ArraySize = 1;
    EShaderStage StageFlags = EShaderStage::None;
};

// One reflected push-constant block, scoped to a single entry point/stage.
// Stages are expected to be merged at the graphics API implementation level (for example, CVulkanGraphicsPipeline)
struct FShaderReflectedPushConstant
{
    std::string Name;
    uint32 Size = 0;
    uint32 Offset = 0;
    EShaderStage StageFlags = EShaderStage::None;
};

struct FShaderReflectionData
{
    std::vector<FShaderReflectedResource> Resources;
    std::vector<FShaderReflectedPushConstant> PushConstants;
};

class CShaderReflection
{
public:
    // Walks a composed, linked slang::ProgramLayout and extracts all resource bindings and push-constant ranges declared across every entry point.
    // This must only be called while the IComponentType the program layout came from is still alive (ProgramLayout is owned by it).
    static FShaderReflectionData Extract(slang::IComponentType* ComposedProgram, slang::ProgramLayout* ProgramLayout);
    
    static void LogShaderReflectionData(const std::string& ShaderName, const FShaderReflectionData& ShaderReflectionData);
private:
    static void ExtractParameter(slang::VariableLayoutReflection* Parameter, EShaderStage StageFlags, FShaderReflectionData& OutReflectionData);
    static void ExtractEntryPointPushConstants(slang::EntryPointLayout* EntryPointLayout, EShaderStage StageFlag, FShaderReflectionData& OutReflectionData);
    
    static EShaderStage GetActualUsageStageFlags(slang::IComponentType* ComposedProgram, slang::ProgramLayout* ProgramLayout, slang::VariableLayoutReflection* GlobalParameter);
    
    static EShaderResourceType GetResourceTypeFromCategory(slang::TypeLayoutReflection* TypeLayout);
    static uint32 GetResourceArraySize(slang::TypeLayoutReflection* TypeLayout);
    static EShaderStage GetShaderStageFlagFromSlangStage(SlangStage Stage);
};

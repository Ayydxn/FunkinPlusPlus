#include "FunkinPCH.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanDebugUtils.h"
#include "VulkanUtils.h"

namespace
{
    vk::DescriptorType GetVulkanDescriptorType(EShaderResourceType ResourceType)
    {
        switch (ResourceType)
        {
            case EShaderResourceType::UniformBuffer:        return vk::DescriptorType::eUniformBuffer;
            case EShaderResourceType::StorageBuffer:        return vk::DescriptorType::eStorageBuffer;
            case EShaderResourceType::Sampler:              return vk::DescriptorType::eSampler;
            case EShaderResourceType::CombinedImageSampler: return vk::DescriptorType::eCombinedImageSampler;
            case EShaderResourceType::SampledImage:         return vk::DescriptorType::eSampledImage;
            case EShaderResourceType::StorageImage:         return vk::DescriptorType::eStorageImage;
        }
        
        verifyFunkinf(false, "Failed to get Vulkan descriptor type for unknown shader resource type!")
        return vk::DescriptorType::eUniformBuffer;
    }
    
    vk::ShaderStageFlags GetVulkanShaderStageFlags(EShaderStage ShaderStageFlags)
    {
        vk::ShaderStageFlags Result;
        
        if (HasShaderStageFlag(ShaderStageFlags, EShaderStage::Vertex))
            Result |= vk::ShaderStageFlagBits::eVertex;
        
        if (HasShaderStageFlag(ShaderStageFlags, EShaderStage::Fragment))
            Result |= vk::ShaderStageFlagBits::eFragment;
        
        if (HasShaderStageFlag(ShaderStageFlags, EShaderStage::Compute))
            Result |= vk::ShaderStageFlagBits::eCompute;
        
        return Result;
    }
    
    vk::Format GetShaderDataTypeVulkanFormat(EShaderDataType DataType)
    {
        switch (DataType)
        {
            case EShaderDataType::Float:     return vk::Format::eR32Sfloat;
            case EShaderDataType::Float2:    return vk::Format::eR32G32Sfloat;
            case EShaderDataType::Float3:    return vk::Format::eR32G32B32Sfloat;
            case EShaderDataType::Float4:    return vk::Format::eR32G32B32A32Sfloat;
                
            case EShaderDataType::Matrix3x3:
            case EShaderDataType::Matrix4x4:
                return vk::Format::eUndefined;
            
            case EShaderDataType::Int:       return vk::Format::eR32Sint;
            case EShaderDataType::Int2:      return vk::Format::eR32G32Sint;
            case EShaderDataType::Int3:      return vk::Format::eR32G32B32Sint;
            case EShaderDataType::Int4:      return vk::Format::eR32G32B32A32Sint;
            
            case EShaderDataType::Boolean:   return vk::Format::eUndefined;
        }
        
        verifyFunkinf(false, "Failed to get Vulkan format for unknown shader data type!")
        return vk::Format::eUndefined;
    }
    
    vk::PrimitiveTopology GetVulkanPrimitiveTopology(EPrimitiveTopology PrimitiveTopology)
    {
        switch (PrimitiveTopology)
        {
            case EPrimitiveTopology::Points:          return vk::PrimitiveTopology::ePointList;
            case EPrimitiveTopology::Lines:           return vk::PrimitiveTopology::eLineList;
            case EPrimitiveTopology::LineStrip:       return vk::PrimitiveTopology::eLineStrip;
            case EPrimitiveTopology::Triangles:       return vk::PrimitiveTopology::eTriangleList;
            case EPrimitiveTopology::TriangleStrip:   return vk::PrimitiveTopology::eTriangleStrip;
            case EPrimitiveTopology::TriangleFan:     return vk::PrimitiveTopology::eTriangleFan;
        }
        
        verifyFunkinf(false, "Failed to get Vulkan primitive topology for unknown primitive topology!")
        return vk::PrimitiveTopology::eTriangleList;
    }
    
    vk::PolygonMode GetVulkanPolygonMode(EFillMode FillMode)
    {
        switch (FillMode)
        {
            case EFillMode::Solid:       return vk::PolygonMode::eFill;
            case EFillMode::Wireframe:   return vk::PolygonMode::eLine;
        }
        
        verifyFunkinf(false, "Failed to get Vulkan polygon mode for unknown fill mode!")
        return vk::PolygonMode::eFill;
    }
    
    vk::CullModeFlagBits GetVulkanCullMode(ECullMode CullMode)
    {
        switch (CullMode)
        {
            case ECullMode::None:            return vk::CullModeFlagBits::eNone;
            case ECullMode::Front:           return vk::CullModeFlagBits::eFront;
            case ECullMode::Back:            return vk::CullModeFlagBits::eBack;
            case ECullMode::FrontAndBack:    return vk::CullModeFlagBits::eFrontAndBack;
        }
        
        verifyFunkinf(false, "Failed to get Vulkan cull mode for unknown cull mode!")
        return vk::CullModeFlagBits::eBack;
    }
    
    vk::FrontFace GetVulkanFrontFace(EFrontFace FrontFace)
    {
        switch (FrontFace)
        {
            case EFrontFace::Clockwise:          return vk::FrontFace::eClockwise;
            case EFrontFace::CounterClockwise:   return vk::FrontFace::eCounterClockwise;
        }
        
        verifyFunkinf(false, "Failed to get Vulkan front face for unknown front face!")
        return vk::FrontFace::eClockwise;
    }
    
    vk::CompareOp GetVulkanCompareOp(ECompareOperation CompareOp)
    {
        switch (CompareOp)
        {
            case ECompareOperation::Never:              return vk::CompareOp::eNever;
            case ECompareOperation::Less:               return vk::CompareOp::eLess;
            case ECompareOperation::Equal:              return vk::CompareOp::eEqual;
            case ECompareOperation::LessOrEqual:        return vk::CompareOp::eLessOrEqual;
            case ECompareOperation::Greater:            return vk::CompareOp::eGreater;
            case ECompareOperation::NotEqual:           return vk::CompareOp::eNotEqual;
            case ECompareOperation::GreaterOrEqual:     return vk::CompareOp::eGreaterOrEqual;
            case ECompareOperation::Always:             return vk::CompareOp::eAlways;
        }
        
        verifyFunkinf(false, "Failed to get Vulkan compare operation for unknown compare operation!")
        return vk::CompareOp::eLess;
    }
    
    vk::BlendFactor GetVulkanBlendFactor(EBlendFactor BlendFactor)
    {
        switch (BlendFactor)
        {
            case EBlendFactor::Zero:             return vk::BlendFactor::eZero;
            case EBlendFactor::One:              return vk::BlendFactor::eOne;
            case EBlendFactor::SrcColor:         return vk::BlendFactor::eSrcColor;
            case EBlendFactor::OneMinusSrcColor: return vk::BlendFactor::eOneMinusSrcColor;
            case EBlendFactor::SrcAlpha:         return vk::BlendFactor::eSrcAlpha;
            case EBlendFactor::OneMinusSrcAlpha: return vk::BlendFactor::eOneMinusSrcAlpha;
            case EBlendFactor::DstColor:         return vk::BlendFactor::eDstColor;
            case EBlendFactor::OneMinusDstColor: return vk::BlendFactor::eOneMinusDstColor;
            case EBlendFactor::DstAlpha:         return vk::BlendFactor::eDstAlpha;
            case EBlendFactor::OneMinusDstAlpha: return vk::BlendFactor::eOneMinusDstAlpha;
        }
        
        verifyFunkinf(false, "Failed to get Vulkan blend factor for unknown blend factor!")
        return vk::BlendFactor::eZero;
    }

    vk::BlendOp GetVulkanBlendOp(EBlendOperation BlendOperation)
    {
        switch (BlendOperation)
        {
            case EBlendOperation::Add:             return vk::BlendOp::eAdd;
            case EBlendOperation::Subtract:        return vk::BlendOp::eSubtract;
            case EBlendOperation::ReverseSubtract: return vk::BlendOp::eReverseSubtract;
            case EBlendOperation::Min:             return vk::BlendOp::eMin;
            case EBlendOperation::Max:             return vk::BlendOp::eMax;
        }
        
        verifyFunkinf(false, "Failed to get Vulkan blend operation for unknown blend operation!")
        return vk::BlendOp::eAdd;
    }
}

CVulkanGraphicsPipeline::CVulkanGraphicsPipeline(const CVulkanContext& VulkanContext, const FGraphicsPipelineDescription& Description)
    : m_VulkanDevice(VulkanContext.GetDevice()), m_GraphicsPipelineDescription(Description)
{
    m_ColorAttachmentFormat = VulkanContext.GetSwapChain()->GetImageFormat();
    
    CreatePipelineLayout();
    Invalidate();
}

CVulkanGraphicsPipeline::~CVulkanGraphicsPipeline()
{
    m_VulkanDevice.GetLogicalDevice().destroyPipeline(m_Pipeline);
    m_VulkanDevice.GetLogicalDevice().destroyPipelineLayout(m_PipelineLayout);
    
    DestroyDescriptorSetLayouts();
}

void CVulkanGraphicsPipeline::Invalidate()
{
    // Meant for cases where a shader gets hot reloaded, and we need to re-create the dependent graphics pipeline.
    // TODO: (Ayydxn) Probably should also account for pipeline layouts once we get there.
    if (m_Pipeline)
    {
        m_VulkanDevice.WaitIdle();
        m_VulkanDevice.GetLogicalDevice().destroyPipeline(m_Pipeline);
    }
    
    const FRasterizerState& RasterizerState = m_GraphicsPipelineDescription.RasterizerState;
    const FDepthStencilState& DepthStencilState = m_GraphicsPipelineDescription.DepthStencilState;
    const FBlendState& BlendState = m_GraphicsPipelineDescription.BlendState;
    
    const auto VulkanShader = std::dynamic_pointer_cast<CVulkanShader>(m_GraphicsPipelineDescription.Shader);
    const auto [Format, bHasStencilComponent] = m_VulkanDevice.GetDepthFormatInfo();
    const std::vector<vk::PipelineShaderStageCreateInfo> ShaderStageCreateInfos = VulkanShader->GetStageCreateInfos();
    
    constexpr std::array<vk::DynamicState, 2> DynamicStates =
    {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
    };
    
    vk::PipelineDynamicStateCreateInfo DynamicStateCreateInfo = {};
    DynamicStateCreateInfo.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
    DynamicStateCreateInfo.dynamicStateCount = static_cast<uint32>(DynamicStates.size());
    DynamicStateCreateInfo.pDynamicStates = DynamicStates.data();
    
    const FVertexBufferLayout& VertexBufferLayout = m_GraphicsPipelineDescription.VertexBufferLayout;
    
    vk::VertexInputBindingDescription VertexInputBindingDescription = {};
    VertexInputBindingDescription.binding = 0;
    VertexInputBindingDescription.stride = VertexBufferLayout.GetStride();
    VertexInputBindingDescription.inputRate = vk::VertexInputRate::eVertex;
    
    std::vector<vk::VertexInputAttributeDescription> VertexInputAttributeDescriptions(VertexBufferLayout.GetElementCount());
    int32 Location = 0;
    
    for (const FVertexBufferElement& VertexBufferElement : VertexBufferLayout.GetElements())
    {
        VertexInputAttributeDescriptions[Location].binding = 0;
        VertexInputAttributeDescriptions[Location].location = Location;
        VertexInputAttributeDescriptions[Location].format = GetShaderDataTypeVulkanFormat(VertexBufferElement.DataType);
        VertexInputAttributeDescriptions[Location].offset = VertexBufferElement.Offset;
        
        Location++;
    }
    
    vk::PipelineVertexInputStateCreateInfo VertexInputStateCreateInfo = {};
    VertexInputStateCreateInfo.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
    VertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    VertexInputStateCreateInfo.pVertexBindingDescriptions = &VertexInputBindingDescription;
    VertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32>(VertexInputAttributeDescriptions.size());
    VertexInputStateCreateInfo.pVertexAttributeDescriptions = VertexInputAttributeDescriptions.data();
    
    vk::PipelineInputAssemblyStateCreateInfo InputAssemblyStateCreateInfo = {};
    InputAssemblyStateCreateInfo.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
    InputAssemblyStateCreateInfo.topology = GetVulkanPrimitiveTopology(m_GraphicsPipelineDescription.PrimitiveTopology);
    InputAssemblyStateCreateInfo.primitiveRestartEnable = vk::False;
    
    // Viewport and scissor counts only. The actual values are supplied dynamically each frame.
    vk::PipelineViewportStateCreateInfo ViewportStateCreateInfo = {};
    ViewportStateCreateInfo.sType = vk::StructureType::ePipelineViewportStateCreateInfo;
    ViewportStateCreateInfo.viewportCount = 1;
    ViewportStateCreateInfo.pViewports = nullptr;
    ViewportStateCreateInfo.scissorCount = 1;
    ViewportStateCreateInfo.pScissors = nullptr;
    
    vk::PipelineRasterizationStateCreateInfo RasterizationStateCreateInfo = {};
    RasterizationStateCreateInfo.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;
    RasterizationStateCreateInfo.depthClampEnable = vk::False;
    RasterizationStateCreateInfo.rasterizerDiscardEnable = vk::False;
    RasterizationStateCreateInfo.polygonMode = GetVulkanPolygonMode(RasterizerState.FillMode);
    RasterizationStateCreateInfo.lineWidth = RasterizerState.LineWidth;
    RasterizationStateCreateInfo.cullMode = GetVulkanCullMode(RasterizerState.CullMode);
    RasterizationStateCreateInfo.frontFace = GetVulkanFrontFace(RasterizerState.FrontFace);
    RasterizationStateCreateInfo.depthBiasEnable = vk::False;
    
    // TODO: (Ayydxn) Multisampling is forced off for now. Make this configurable.
    vk::PipelineMultisampleStateCreateInfo MultisampleStateCreateInfo = {};
    MultisampleStateCreateInfo.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
    MultisampleStateCreateInfo.sampleShadingEnable = vk::False;
    MultisampleStateCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
    
    vk::PipelineDepthStencilStateCreateInfo DepthStencilStateCreateInfo = {};
    DepthStencilStateCreateInfo.sType = vk::StructureType::ePipelineDepthStencilStateCreateInfo;
    DepthStencilStateCreateInfo.depthTestEnable = DepthStencilState.bEnableDepthTesting;
    DepthStencilStateCreateInfo.depthWriteEnable = DepthStencilState.bEnableDepthWriting;
    DepthStencilStateCreateInfo.depthCompareOp = GetVulkanCompareOp(DepthStencilState.DepthCompareOp);
    DepthStencilStateCreateInfo.depthBoundsTestEnable = false;
    DepthStencilStateCreateInfo.stencilTestEnable = false;
    
    vk::PipelineColorBlendAttachmentState ColorBlendAttachmentState = {};
    ColorBlendAttachmentState.blendEnable = BlendState.bEnableBlending;
    ColorBlendAttachmentState.srcColorBlendFactor = GetVulkanBlendFactor(BlendState.SrcColorBlendFactor);
    ColorBlendAttachmentState.dstColorBlendFactor = GetVulkanBlendFactor(BlendState.DstColorBlendFactor);
    ColorBlendAttachmentState.colorBlendOp = GetVulkanBlendOp(BlendState.ColorBlendOperation);
    ColorBlendAttachmentState.srcAlphaBlendFactor = GetVulkanBlendFactor(BlendState.SrcAlphaBlendFactor);
    ColorBlendAttachmentState.dstAlphaBlendFactor = GetVulkanBlendFactor(BlendState.DstAlphaBlendFactor);
    ColorBlendAttachmentState.alphaBlendOp = GetVulkanBlendOp(BlendState.AlphaBlendOperation);
    ColorBlendAttachmentState.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
        vk::ColorComponentFlagBits::eA;
    
    vk::PipelineColorBlendStateCreateInfo ColorBlendStateCreateInfo = {};
    ColorBlendStateCreateInfo.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
    ColorBlendStateCreateInfo.logicOpEnable = vk::False;
    ColorBlendStateCreateInfo.logicOp = vk::LogicOp::eCopy;
    ColorBlendStateCreateInfo.attachmentCount = 1;
    ColorBlendStateCreateInfo.pAttachments = &ColorBlendAttachmentState;
    ColorBlendStateCreateInfo.blendConstants = std::array<float, 4> { 0.0f, 0.0f, 0.0f, 0.0f };
    
    vk::PipelineRenderingCreateInfo PipelineRenderingCreateInfo = {};
    PipelineRenderingCreateInfo.sType = vk::StructureType::ePipelineRenderingCreateInfo;
    PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    PipelineRenderingCreateInfo.pColorAttachmentFormats = &m_ColorAttachmentFormat;
    PipelineRenderingCreateInfo.depthAttachmentFormat = Format;
    
    if (bHasStencilComponent)
        PipelineRenderingCreateInfo.stencilAttachmentFormat = Format;
    
    vk::GraphicsPipelineCreateInfo GraphicsPipelineCreateInfo = {};
    GraphicsPipelineCreateInfo.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
    GraphicsPipelineCreateInfo.stageCount = static_cast<uint32>(ShaderStageCreateInfos.size());
    GraphicsPipelineCreateInfo.pStages = ShaderStageCreateInfos.data();
    GraphicsPipelineCreateInfo.pVertexInputState = &VertexInputStateCreateInfo;
    GraphicsPipelineCreateInfo.pInputAssemblyState = &InputAssemblyStateCreateInfo;
    GraphicsPipelineCreateInfo.pViewportState = &ViewportStateCreateInfo;
    GraphicsPipelineCreateInfo.pRasterizationState = &RasterizationStateCreateInfo;
    GraphicsPipelineCreateInfo.pMultisampleState = &MultisampleStateCreateInfo;
    GraphicsPipelineCreateInfo.pDepthStencilState = &DepthStencilStateCreateInfo;
    GraphicsPipelineCreateInfo.pColorBlendState = &ColorBlendStateCreateInfo;
    GraphicsPipelineCreateInfo.pDynamicState = &DynamicStateCreateInfo;
    GraphicsPipelineCreateInfo.layout = m_PipelineLayout;
    GraphicsPipelineCreateInfo.renderPass = nullptr;
    GraphicsPipelineCreateInfo.basePipelineHandle = nullptr;
    GraphicsPipelineCreateInfo.basePipelineIndex = -1;
    GraphicsPipelineCreateInfo.pNext = &PipelineRenderingCreateInfo;
    GraphicsPipelineCreateInfo.flags = vk::PipelineCreateFlags();
    
    VK_CHECK_RESULT(m_VulkanDevice.GetLogicalDevice().createGraphicsPipeline(m_VulkanDevice.GetPipelineCache(), GraphicsPipelineCreateInfo), m_Pipeline,
        "Failed to create Vulkan graphics pipeline!")
}

void CVulkanGraphicsPipeline::CreatePipelineLayout()
{
    const vk::Device LogicalDevice = m_VulkanDevice.GetLogicalDevice();
    
    const auto VulkanShader = std::dynamic_pointer_cast<CVulkanShader>(m_GraphicsPipelineDescription.Shader);
    verifyFunkinf(VulkanShader, "Failed to create Vulkan graphics pipeline layout! Shader is not a valid Vulkan shader instance!")
    
    const FShaderReflectionData& ShaderReflectionData = VulkanShader->GetReflectionData();
    std::map<uint32, std::vector<vk::DescriptorSetLayoutBinding>> BindingsBySet;
    
    for (const FShaderReflectedResource& ShaderReflectedResource : ShaderReflectionData.Resources)
    {
        vk::DescriptorSetLayoutBinding DescriptorSetLayoutBinding = {};
        DescriptorSetLayoutBinding.binding = ShaderReflectedResource.Binding;
        DescriptorSetLayoutBinding.descriptorType = GetVulkanDescriptorType(ShaderReflectedResource.Type);
        DescriptorSetLayoutBinding.descriptorCount = 1;
        DescriptorSetLayoutBinding.stageFlags = GetVulkanShaderStageFlags(ShaderReflectedResource.StageFlags);
        
        BindingsBySet[ShaderReflectedResource.Set].push_back(DescriptorSetLayoutBinding);
    }
    
    // Sets must be created contiguously from 0 for VkPipelineLayoutCreateInfo::pSetLayouts.
    // So, any gap in reflected set numbers (e.g. Set 0 and Set 2 used, Set 1 not) is filled with an empty layout rather than left out.
    const uint32 SetCount = BindingsBySet.empty() ? 0 : (BindingsBySet.rbegin()->first + 1);
    m_DescriptorSetLayouts.reserve(SetCount);
    
    for (uint32 SetIndex = 0; SetIndex < SetCount; ++SetIndex)
    {
        const std::vector<vk::DescriptorSetLayoutBinding>& SetBindings = BindingsBySet[SetIndex];
        
        vk::DescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo = {};
        DescriptorSetLayoutCreateInfo.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo;
        DescriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32>(SetBindings.size());
        DescriptorSetLayoutCreateInfo.pBindings = SetBindings.data();
        DescriptorSetLayoutCreateInfo.flags = vk::DescriptorSetLayoutCreateFlags();
        
        vk::DescriptorSetLayout DescriptorSetLayout;
        VK_CHECK_RESULT(LogicalDevice.createDescriptorSetLayout(DescriptorSetLayoutCreateInfo), DescriptorSetLayout,
            "Failed to create Vulkan descriptor set layout for set {} of shader '{}'!", SetIndex, VulkanShader->GetName())
        
        m_DescriptorSetLayouts.push_back(DescriptorSetLayout);
    }
    
    // Slang scopes each push-constant block to the entry point/stage it was declared in, so the same logical push-constant
    // range (matching offset) can show up once per stage that uses it. Vulkan wants one VkPushConstantRange per byte range,
    // with a single stage mask covering every stage that touches it - so ranges sharing an offset+size are merged here.
    std::vector<vk::PushConstantRange> PushConstantRanges;
    
    for (const FShaderReflectedPushConstant& PushConstant : ShaderReflectionData.PushConstants)
    {
        bool bMergedIntoExisting = false;
        
        for (vk::PushConstantRange& ExistingRange : PushConstantRanges)
        {
            if (ExistingRange.offset == PushConstant.Offset && ExistingRange.size == PushConstant.Size)
            {
                ExistingRange.stageFlags |= GetVulkanShaderStageFlags(PushConstant.StageFlags);
                bMergedIntoExisting = true;
                break;
            }
        }
        
        if (bMergedIntoExisting)
            continue;
        
        vk::PushConstantRange PushConstantRange = {};
        PushConstantRange.stageFlags = GetVulkanShaderStageFlags(PushConstant.StageFlags);
        PushConstantRange.offset = PushConstant.Offset;
        PushConstantRange.size = PushConstant.Size;
        
        PushConstantRanges.push_back(PushConstantRange);
    }
    
    vk::PipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
    PipelineLayoutCreateInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
    PipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32>(m_DescriptorSetLayouts.size());
    PipelineLayoutCreateInfo.pSetLayouts = m_DescriptorSetLayouts.data();
    PipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32>(PushConstantRanges.size());
    PipelineLayoutCreateInfo.pPushConstantRanges = PushConstantRanges.data();
    PipelineLayoutCreateInfo.flags = vk::PipelineLayoutCreateFlags();
    
    VK_CHECK_RESULT(LogicalDevice.createPipelineLayout(PipelineLayoutCreateInfo), m_PipelineLayout, "Failed to create Vulkan graphics pipeline layout!")
}

void CVulkanGraphicsPipeline::DestroyDescriptorSetLayouts()
{
    const vk::Device LogicalDevice = m_VulkanDevice.GetLogicalDevice();
    
    for (const vk::DescriptorSetLayout DescriptorSetLayout : m_DescriptorSetLayouts)
        LogicalDevice.destroyDescriptorSetLayout(DescriptorSetLayout);
    
    m_DescriptorSetLayouts.clear();
}

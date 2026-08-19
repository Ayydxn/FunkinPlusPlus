#include "FunkinPCH.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanDebugUtils.h"

namespace
{
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
}

CVulkanGraphicsPipeline::CVulkanGraphicsPipeline(const CVulkanContext& VulkanContext, const FGraphicsPipelineDescription& Description)
    : m_VulkanDevice(VulkanContext.GetDevice()), m_GraphicsPipelineDescription(Description)
{
    m_ColorAttachmentFormat = VulkanContext.GetSwapChain()->GetImageFormat();
    
    CreatePipelineLayoutAndCache();
    Invalidate();
}

CVulkanGraphicsPipeline::~CVulkanGraphicsPipeline()
{
    m_VulkanDevice.GetLogicalDevice().destroyPipeline(m_Pipeline);
    m_VulkanDevice.GetLogicalDevice().destroyPipelineLayout(m_PipelineLayout);
    m_VulkanDevice.GetLogicalDevice().destroyPipelineCache(m_PipelineCache);
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
    
    const auto VulkanShader = std::dynamic_pointer_cast<CVulkanShader>(m_GraphicsPipelineDescription.Shader);
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
    InputAssemblyStateCreateInfo.topology = vk::PrimitiveTopology::eTriangleList;
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
    RasterizationStateCreateInfo.polygonMode = vk::PolygonMode::eFill;
    RasterizationStateCreateInfo.lineWidth = 1.0f;
    RasterizationStateCreateInfo.cullMode = vk::CullModeFlagBits::eBack;
    RasterizationStateCreateInfo.frontFace = vk::FrontFace::eClockwise;
    RasterizationStateCreateInfo.depthBiasEnable = vk::False;
    
    // TODO: (Ayydxn) Multisampling is forced off for now. Make this configurable.
    vk::PipelineMultisampleStateCreateInfo MultisampleStateCreateInfo = {};
    MultisampleStateCreateInfo.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
    MultisampleStateCreateInfo.sampleShadingEnable = vk::False;
    MultisampleStateCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineColorBlendAttachmentState ColorBlendAttachmentState = {};
    ColorBlendAttachmentState.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
        vk::ColorComponentFlagBits::eA;
    ColorBlendAttachmentState.blendEnable = vk::False;
    
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
    PipelineRenderingCreateInfo.depthAttachmentFormat = vk::Format::eUndefined;
    PipelineRenderingCreateInfo.stencilAttachmentFormat = vk::Format::eUndefined;
    
    vk::GraphicsPipelineCreateInfo GraphicsPipelineCreateInfo = {};
    GraphicsPipelineCreateInfo.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
    GraphicsPipelineCreateInfo.stageCount = static_cast<uint32>(ShaderStageCreateInfos.size());
    GraphicsPipelineCreateInfo.pStages = ShaderStageCreateInfos.data();
    GraphicsPipelineCreateInfo.pVertexInputState = &VertexInputStateCreateInfo;
    GraphicsPipelineCreateInfo.pInputAssemblyState = &InputAssemblyStateCreateInfo;
    GraphicsPipelineCreateInfo.pViewportState = &ViewportStateCreateInfo;
    GraphicsPipelineCreateInfo.pRasterizationState = &RasterizationStateCreateInfo;
    GraphicsPipelineCreateInfo.pMultisampleState = &MultisampleStateCreateInfo;
    GraphicsPipelineCreateInfo.pDepthStencilState = nullptr;
    GraphicsPipelineCreateInfo.pColorBlendState = &ColorBlendStateCreateInfo;
    GraphicsPipelineCreateInfo.pDynamicState = &DynamicStateCreateInfo;
    GraphicsPipelineCreateInfo.layout = m_PipelineLayout;
    GraphicsPipelineCreateInfo.renderPass = nullptr;
    GraphicsPipelineCreateInfo.basePipelineHandle = nullptr;
    GraphicsPipelineCreateInfo.basePipelineIndex = -1;
    GraphicsPipelineCreateInfo.pNext = &PipelineRenderingCreateInfo;
    GraphicsPipelineCreateInfo.flags = vk::PipelineCreateFlags();
    
    VK_CHECK_RESULT(m_VulkanDevice.GetLogicalDevice().createGraphicsPipeline(m_PipelineCache, GraphicsPipelineCreateInfo), m_Pipeline,
        "Failed to create Vulkan graphics pipeline!")
}

void CVulkanGraphicsPipeline::CreatePipelineLayoutAndCache()
{
    const vk::Device LogicalDevice = m_VulkanDevice.GetLogicalDevice();
    
    vk::PipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
    PipelineLayoutCreateInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
    PipelineLayoutCreateInfo.flags = vk::PipelineLayoutCreateFlags();
    
    vk::PipelineCacheCreateInfo PipelineCacheCreateInfo = {};
    PipelineCacheCreateInfo.sType = vk::StructureType::ePipelineCacheCreateInfo;
    PipelineCacheCreateInfo.flags = vk::PipelineCacheCreateFlags();
    
    VK_CHECK_RESULT(LogicalDevice.createPipelineLayout(PipelineLayoutCreateInfo), m_PipelineLayout, "Failed to create Vulkan graphics pipeline layout!")
    VK_CHECK_RESULT(LogicalDevice.createPipelineCache(PipelineCacheCreateInfo), m_PipelineCache, "Failed to create Vulkan graphics pipeline cache!")
}

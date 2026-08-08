#pragma once

#include "VulkanContext.h"
#include "VulkanShader.h"
#include "RHICore/GraphicsPipeline.h"

class CVulkanGraphicsPipeline final : public IGraphicsPipeline
{
public:
    CVulkanGraphicsPipeline(const CVulkanContext& VulkanContext, const FGraphicsPipelineDescription& Description);
    ~CVulkanGraphicsPipeline() override;
    
    void Invalidate() override;

    vk::Pipeline GetHandle() const { return m_Pipeline; }
    vk::PipelineLayout GetLayout() const { return m_PipelineLayout; }
private:
    void CreatePipelineLayoutAndCache();
private:
    CVulkanDevice& m_VulkanDevice;
    const FGraphicsPipelineDescription& m_GraphicsPipelineDescription;
    
    vk::Format m_ColorAttachmentFormat;
    vk::PipelineLayout m_PipelineLayout;
    vk::PipelineCache m_PipelineCache;
    vk::Pipeline m_Pipeline;
};

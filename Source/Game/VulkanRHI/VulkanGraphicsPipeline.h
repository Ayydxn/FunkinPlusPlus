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
    
    vk::DescriptorSetLayout GetDescriptorSetLayout(uint32 SetIndex) const
    {
        return SetIndex < m_DescriptorSetLayouts.size() ? m_DescriptorSetLayouts[SetIndex] : VK_NULL_HANDLE;
    }
private:
    void CreatePipelineLayout();
    
    void DestroyDescriptorSetLayouts();
private:
    CVulkanDevice& m_VulkanDevice;
    FGraphicsPipelineDescription m_GraphicsPipelineDescription;
    
    std::vector<vk::DescriptorSetLayout> m_DescriptorSetLayouts;
    
    vk::Format m_ColorAttachmentFormat;
    vk::PipelineLayout m_PipelineLayout;
    vk::Pipeline m_Pipeline;
};

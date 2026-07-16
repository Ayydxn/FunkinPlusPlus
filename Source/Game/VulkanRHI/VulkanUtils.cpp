#include "FunkinPCH.h"
#include "VulkanUtils.h"

void CVulkanUtils::TransitionImageLayout(const vk::CommandBuffer& CommandBuffer, const vk::Image& Image, vk::PipelineStageFlags2 SrcStageMask,
    vk::PipelineStageFlags2 DstStageMask, vk::AccessFlags2 SrcAccessMask, vk::AccessFlags2 DstAccessMask, vk::ImageLayout CurrentImageLayout,
    vk::ImageLayout NewImageLayout)
{
    vk::ImageAspectFlags ImageAspectMask = vk::ImageAspectFlagBits::eColor;
    if (CurrentImageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
        ImageAspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
    
    vk::ImageSubresourceRange ImageSubresourceRange;
    ImageSubresourceRange.aspectMask = ImageAspectMask;
    ImageSubresourceRange.baseMipLevel = 0;
    ImageSubresourceRange.levelCount = 1;
    ImageSubresourceRange.baseArrayLayer = 0;
    ImageSubresourceRange.layerCount = 1;
    
    vk::ImageMemoryBarrier2 ImageMemoryBarrier = {};
    ImageMemoryBarrier.sType = vk::StructureType::eImageMemoryBarrier2;
    ImageMemoryBarrier.srcStageMask = SrcStageMask;
    ImageMemoryBarrier.srcAccessMask = SrcAccessMask;
    ImageMemoryBarrier.dstStageMask = DstStageMask;
    ImageMemoryBarrier.dstAccessMask = DstAccessMask;
    ImageMemoryBarrier.oldLayout = CurrentImageLayout;
    ImageMemoryBarrier.newLayout = NewImageLayout;
    ImageMemoryBarrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
    ImageMemoryBarrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
    ImageMemoryBarrier.image = Image;
    ImageMemoryBarrier.subresourceRange = ImageSubresourceRange;
    
    vk::DependencyInfo DependencyInfo = {};
    DependencyInfo.sType = vk::StructureType::eDependencyInfo;
    DependencyInfo.imageMemoryBarrierCount = 1;
    DependencyInfo.pImageMemoryBarriers = &ImageMemoryBarrier;
    
    CommandBuffer.pipelineBarrier2(DependencyInfo);
}

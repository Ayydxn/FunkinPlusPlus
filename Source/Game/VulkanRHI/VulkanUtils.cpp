#include "FunkinPCH.h"
#include "VulkanUtils.h"

void CVulkanUtils::TransitionImageLayout(vk::CommandBuffer CommandBuffer, vk::Image Image, vk::PipelineStageFlags2 SrcStageMask, vk::PipelineStageFlags2 DstStageMask,
    vk::AccessFlags2 SrcAccessMask, vk::AccessFlags2 DstAccessMask, vk::ImageLayout CurrentImageLayout, vk::ImageLayout NewImageLayout)
{
    vk::ImageAspectFlags ImageAspectMask = vk::ImageAspectFlagBits::eColor;
    if (NewImageLayout == vk::ImageLayout::eDepthAttachmentOptimal)
        ImageAspectMask = vk::ImageAspectFlagBits::eDepth;
    
    if (NewImageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
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

void CVulkanUtils::CopyBufferToImage(vk::CommandBuffer CommandBuffer, vk::Buffer Buffer, vk::Image Image, uint32 ImageWidth, uint32 ImageHeight,
    vk::ImageLayout ImageLayout)
{
    vk::BufferImageCopy BufferImageCopy;
    BufferImageCopy.bufferOffset = 0;
    BufferImageCopy.bufferRowLength = 0;
    BufferImageCopy.bufferImageHeight = 0;
    BufferImageCopy.imageOffset = vk::Offset3D(0, 0, 0);
    BufferImageCopy.imageExtent = vk::Extent3D(ImageWidth, ImageHeight, 1);
    
    BufferImageCopy.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    BufferImageCopy.imageSubresource.mipLevel = 0;
    BufferImageCopy.imageSubresource.baseArrayLayer = 0;
    BufferImageCopy.imageSubresource.layerCount = 1;
    
    CommandBuffer.copyBufferToImage(Buffer, Image, ImageLayout, BufferImageCopy);
}

bool CVulkanUtils::DoesFormatHaveStencilComponent(vk::Format Format)
{
    constexpr std::array<vk::Format, 2> StencilFormats
    {
        vk::Format::eD32SfloatS8Uint,
        vk::Format::eD24UnormS8Uint
    };
    
    return std::ranges::find(StencilFormats, Format) != StencilFormats.end();
}

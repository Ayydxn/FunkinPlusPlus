#pragma once

#include "VulkanIncludes.h"

class CVulkanUtils
{
public:
    static void TransitionImageLayout(vk::CommandBuffer CommandBuffer, vk::Image Image,
        vk::PipelineStageFlags2 SrcStageMask, vk::PipelineStageFlags2 DstStageMask, vk::AccessFlags2 SrcAccessMask,
        vk::AccessFlags2 DstAccessMask, vk::ImageLayout CurrentImageLayout, vk::ImageLayout NewImageLayout);
    
    static void CopyBufferToImage(vk::CommandBuffer CommandBuffer, vk::Buffer Buffer, vk::Image Image, uint32 ImageWidth, uint32 ImageHeight,
        vk::ImageLayout ImageLayout);
    
    static bool DoesFormatHaveStencilComponent(vk::Format Format);
};

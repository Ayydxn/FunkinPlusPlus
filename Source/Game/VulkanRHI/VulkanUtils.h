#pragma once

#include "VulkanIncludes.h"

class CVulkanUtils
{
public:
    static void TransitionImageLayout(const vk::CommandBuffer& CommandBuffer, const vk::Image& Image,
        vk::PipelineStageFlags2 SrcStageMask, vk::PipelineStageFlags2 DstStageMask, vk::AccessFlags2 SrcAccessMask,
        vk::AccessFlags2 DstAccessMask, vk::ImageLayout CurrentImageLayout, vk::ImageLayout NewImageLayout);
};

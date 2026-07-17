#pragma once

#include "VulkanIncludes.h"

#ifdef TRACY_ENABLE
     #define TRACY_VK_USE_SYMBOL_TABLE
     #include <tracy/TracyVulkan.hpp>

    #define FUNKIN_PROFILE_VULKAN_ZONE(TracyContext, CommandBuffer, Name) TracyVkZone(TracyContext, CommandBuffer, Name)
    #define FUNKIN_PROFILE_VULKAN_COLLECT(TracyContext, CommandBuffer) TracyVkCollect(TracyContext, CommandBuffer)
#else
    #define FUNKIN_PROFILE_VULKAN_ZONE(TracyContext, CommandBuffer, Name)
    #define FUNKIN_PROFILE_VULKAN_COLLECT(TracyContext, CommandBuffer)
#endif

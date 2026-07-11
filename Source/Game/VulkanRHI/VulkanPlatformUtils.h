#pragma once

#include "VulkanIncludes.h"
#include "Application/Window.h"

class CVulkanPlatformUtils
{
public:
    static vk::SurfaceKHR CreateSurface(const vk::Instance& VulkanInstance, const FNativeWindowHandle& NativeWindowHandle);
};

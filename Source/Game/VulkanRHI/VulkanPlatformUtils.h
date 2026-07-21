#pragma once

#include "VulkanIncludes.h"
#include "Application/Window.h"

class CVulkanPlatformUtils
{
public:
    static vk::SurfaceKHR CreateSurface(vk::Instance VulkanInstance, const FNativeWindowHandle& NativeWindowHandle);
};

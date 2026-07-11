#pragma once

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_NO_EXCEPTIONS

#ifdef FUNKIN_PLATFORM_WIN64
    #define VK_USE_PLATFORM_WIN32_KHR
#elif defined(FUNKIN_PLATFORM_MAC)
    #define VK_USE_PLATFORM_MACOS_MVK
#elif defined(FUNKIN_PLATFORM_LINUX)
    #define VK_USE_PLATFORM_XLIB_KHR
    #define VK_USE_PLATFORM_WAYLAND_KHR
#endif

#include <vulkan/vulkan.hpp>

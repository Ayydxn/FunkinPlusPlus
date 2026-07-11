#include "FunkinPCH.h"
#include "VulkanPlatformUtils.h"
#include "VulkanDebugUtils.h"
#include "RHICore/RHIContext.h"

vk::SurfaceKHR CVulkanPlatformUtils::CreateSurface(const vk::Instance& VulkanInstance, const FNativeWindowHandle& NativeWindowHandle)
{
    vk::SurfaceKHR Surface;
    
    switch (NativeWindowHandle.Platform)
    {
        case FNativeWindowHandle::EPlatform::Win32:
        {
            #ifdef FUNKIN_PLATFORM_WIN64
                vk::Win32SurfaceCreateInfoKHR SurfaceCreateInfo = {};
                SurfaceCreateInfo.sType = vk::StructureType::eWin32SurfaceCreateInfoKHR;
                SurfaceCreateInfo.hinstance = static_cast<HINSTANCE>(NativeWindowHandle.Win32.HInstance);
                SurfaceCreateInfo.hwnd = static_cast<HWND>(NativeWindowHandle.Win32.Hwnd);
                SurfaceCreateInfo.flags = vk::Win32SurfaceCreateFlagsKHR();
                
                VK_CHECK_RESULT(VulkanInstance.createWin32SurfaceKHR(SurfaceCreateInfo, nullptr), Surface, "Failed to create Win32 Vulkan surface!")
            #endif
            
            break;
        }
            
        case FNativeWindowHandle::EPlatform::Cocoa: verifyFunkinf(false, "Failed to create Vulkan surface! MacOS is unsupported!") break;
        case FNativeWindowHandle::EPlatform::X11: verifyFunkinf(false, "Failed to create Vulkan surface! Linux (X11) is unsupported!") break;
        case FNativeWindowHandle::EPlatform::Wayland: verifyFunkinf(false, "Failed to create Vulkan surface! Linux (Wayland) is unsupported!") break;
    }
    
    return Surface;
}

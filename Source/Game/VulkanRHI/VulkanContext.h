#pragma once

#include "VulkanDevice.h"
#include "VulkanIncludes.h"
#include "VulkanSwapChain.h"
#include "Application/Window.h"
#include "RHICore/RHIContext.h"

class CVulkanContext final : public IRHIContext
{
public:
    CVulkanContext() = default;
    ~CVulkanContext() override = default;
    
    bool Initialize(uint32 WindowID, const FNativeWindowHandle& NativeWindowHandle, uint32 InitialWindowWidth, uint32 InitialWindowHeight, bool bRequestVSync) override;
    void Destroy() override;
    
    void OnWindowResized(uint32 WindowID, uint32 NewWidth, uint32 NewHeight) override;
private:
    void CreateDebugMessenger();
    void PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& DebugMessengerCreateInfo);
    
    std::vector<const char*> GetRequiredInstanceExtensions();
private:
    static bool bEnableValidationLayers;
    
    std::shared_ptr<CVulkanDevice> m_Device;
    std::unique_ptr<CVulkanSwapChain> m_MainWindowSwapChain;
    
    vk::Instance m_Instance;
    vk::DebugUtilsMessengerEXT m_DebugUtilsMessenger;
};

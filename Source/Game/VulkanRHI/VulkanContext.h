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
    
    bool Initialize(const FNativeWindowHandle& NativeWindowHandle, uint32 InitialWindowWidth, uint32 InitialWindowHeight, bool bRequestVSync) override;
    void Destroy() override;
    
    void OnWindowResized(uint32 NewWidth, uint32 NewHeight) override;
    
    CVulkanDevice& GetDevice() const { return *m_Device; }
    std::shared_ptr<CVulkanSwapChain> GetSwapChain() const { return m_SwapChain; }
public:
    // TODO: (Ayydxn) Once game settings exist, read this from there instead of hardcoding it.
    static constexpr uint32 DefaultFramesInFlight = 3;
private:
    void CreateDebugMessenger();
    void PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& DebugMessengerCreateInfo);
    
    std::vector<const char*> GetRequiredInstanceExtensions();
private:
    static bool bEnableValidationLayers;
    
    std::unique_ptr<CVulkanDevice> m_Device;
    std::shared_ptr<CVulkanSwapChain> m_SwapChain;
    
    vk::Instance m_Instance;
    vk::DebugUtilsMessengerEXT m_DebugUtilsMessenger;
};

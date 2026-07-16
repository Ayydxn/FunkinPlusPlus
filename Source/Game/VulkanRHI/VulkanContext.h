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
    
    bool RegisterWindow(uint32 WindowID, const FNativeWindowHandle& NativeWindowHandle, uint32 InitialWidth, uint32 InitialHeight, bool bRequestVSync) override;
    void UnregisterWindow(uint32 WindowID) override;
    
    void OnWindowResized(uint32 WindowID, uint32 NewWidth, uint32 NewHeight) override;
    
    CVulkanDevice& GetDevice() const { return *m_Device; }
    CVulkanSwapChain* GetSwapChain(uint32 WindowID) const;
    
    // (Ayydxn) Always returns the main game window first. Secondaries come after.
    std::vector<uint32> GetOrderedWindowIDs() const;
private:
    void CreateDebugMessenger();
    void PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& DebugMessengerCreateInfo);
    
    std::vector<const char*> GetRequiredInstanceExtensions();
private:
    // TODO: (Ayydxn) Once game settings exist, read this from there instead of hardcoding it.
    static constexpr uint32 DefaultFramesInFlight = 3;
    
    static bool bEnableValidationLayers;
    
    std::unordered_map<uint32, std::unique_ptr<CVulkanSwapChain>> m_WindowSwapChains;
    
    std::shared_ptr<CVulkanDevice> m_Device;
    
    vk::Instance m_Instance;
    vk::DebugUtilsMessengerEXT m_DebugUtilsMessenger;
    
    uint32 m_MainWindowID = 0;
};

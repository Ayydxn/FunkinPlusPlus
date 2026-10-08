#pragma once

#include "VulkanDescriptorAllocator.h"
#include "VulkanDevice.h"
#include "VulkanMemoryAllocator.h"
#include "VulkanSwapChain.h"
#include "Application/Window.h"
#include "RHICore/RHIContext.h"

class CVulkanContext final : public IRHIContext
{
public:
    CVulkanContext() = default;
    ~CVulkanContext() override = default;
    
    FRHIInitializationResult Initialize(const FNativeWindowHandle& NativeWindowHandle, uint32 InitialWindowWidth, uint32 InitialWindowHeight, bool bRequestVSync) override;
    void Destroy() override;
    
    void WaitIdle() override;
    
    void OnWindowResized(uint32 NewWidth, uint32 NewHeight) override;
    
    vk::Instance GetInstance() const { return m_Instance; }
    CVulkanDevice& GetDevice() const { return *m_Device; }
    CVulkanMemoryAllocator& GetMemoryAllocator() const { return *m_MemoryAllocator; }
    std::shared_ptr<CVulkanSwapChain> GetSwapChain() const { return m_SwapChain; }
    CVulkanDescriptorAllocator& GetDescriptorAllocator() const { return *m_DescriptorAllocator; }
public:
    // TODO: (Ayydxn) Once game settings exist, read this from there instead of hardcoding it.
    static constexpr uint32 DefaultFramesInFlight = 3;
    
    // The number of sampled image descriptors in the texture table the batch renderer indexes per instance.
    // The table lives in an update-after-bind set. So, it's the update-after-bind limits (not the regular maxPerStage* ones, which are as low as 200 on some Intel iGPUs) that decide whether it fits.
    // Physical device selection rejects any GPU whose update-after-bind limits are below this, so the table can always be created at this size.
    static constexpr uint32 TextureTableCapacity = 1024;
    
    // Largest texture dimension we require the GPU to support. The game's character atlases go up to 8192 pixels on a side, while Vulkan only guarantees 4096.
    static constexpr uint32 RequiredMaxTextureDimension = 8192;
private:
    void CreateDebugMessenger();
    void PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& DebugMessengerCreateInfo);
    
    std::vector<const char*> GetRequiredInstanceExtensions();
private:
    static bool bEnableValidationLayers;
    
    std::unique_ptr<CVulkanDevice> m_Device;
    std::unique_ptr<CVulkanMemoryAllocator> m_MemoryAllocator;
    std::shared_ptr<CVulkanSwapChain> m_SwapChain;
    std::unique_ptr<CVulkanDescriptorAllocator> m_DescriptorAllocator;
    
    vk::Instance m_Instance;
    vk::DebugUtilsMessengerEXT m_DebugUtilsMessenger;
};

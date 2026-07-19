#pragma once

#include "VulkanIncludes.h"

namespace tracy { class VkCtx; }

struct FVulkanDeviceInfo
{
    vk::PhysicalDeviceProperties PhysicalDeviceProperties;
    std::string VendorName;
    std::string DriverVersion;
    std::string VulkanAPIVersion;
};

struct FQueueFamilyIndices
{
    std::optional<uint32> GraphicsFamily;
    std::optional<uint32> PresentFamily;

    bool IsComplete() const { return GraphicsFamily.has_value() && PresentFamily.has_value(); }
};

struct FSubmitInfo
{
    vk::CommandBuffer CommandBuffer;
    vk::Semaphore WaitSemaphore;
    vk::PipelineStageFlags WaitStage;
    vk::Semaphore SignalSemaphore;
    vk::Fence SignalFence;
};

struct FPresentInfo
{
    vk::SwapchainKHR SwapChain;
    uint32 ImageIndex;
    vk::Semaphore WaitSemaphore;
};

class CVulkanDevice
{
public:
    explicit CVulkanDevice(const vk::Instance& VulkanInstance, const vk::SurfaceKHR& ProbeSurface);
    
    void Destroy() const;
    
    void RegisterWindow(uint32 WindowID, uint32 FramesInFlight);
    void UnregisterWindow(uint32 WindowID);
    
    void WaitIdle() const;
    vk::Result Submit(const FSubmitInfo& SubmitInfo) const;
    vk::Result Present(const FPresentInfo& PresentInfo) const;
    
    const vk::PhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; }
    const vk::Device& GetLogicalDevice() const { return m_LogicalDevice; }
    const FQueueFamilyIndices GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }
    const vk::Queue& GetGraphicsQueue() const { return m_GraphicsQueue; }
    const vk::Queue& GetPresentQueue() const { return m_PresentQueue; }
    const FVulkanDeviceInfo& GetDeviceInfo() const { return m_DeviceInfo; }
    vk::CommandBuffer GetCommandBuffer(uint32 WindowID, uint32 FrameIndex) const;
    tracy::VkCtx* GetTracyContext() const { return m_TracyVulkanContext; }
private:
    void SelectPhysicalDevice(const vk::Instance& VulkanInstance, const vk::SurfaceKHR& ProbeSurface);
    void CreateLogicalDevice(const vk::PhysicalDevice& PhysicalDevice);
    void CreateCommandPool();
    void InitializeTracyContext(const vk::Instance& VulkanInstance);
    
    bool IsPhysicalDeviceSuitable(const vk::PhysicalDevice& PhysicalDevice, const vk::SurfaceKHR& ProbeSurface);
    bool DoesPhysicalDeviceSupportRequiredExtensions(const vk::PhysicalDevice& PhysicalDevice);
    bool DoesPhysicalDeviceSupportRequiredFeatures(const vk::PhysicalDevice& PhysicalDevice);
    uint32 RatePhysicalDevice(const vk::PhysicalDevice& PhysicalDevice);
    
    FQueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice& PhysicalDevice, const vk::SurfaceKHR& ProbeSurface);
    
    // Physical device helpers
    std::string GetVendorNameFromID(uint32 VendorID);
    std::string UnpackDriverVersion(uint32 VendorID, uint32 DriverVersion);
    std::string UnpackVulkanAPIVersion(uint32 VulkanAPIVersion);
private:
    std::unordered_map<uint32, std::vector<vk::CommandBuffer>> m_WindowCommandBuffers;
    
    FVulkanDeviceInfo m_DeviceInfo;
    FQueueFamilyIndices m_QueueFamilyIndices;
    
    // Always null in Distribution builds. If using this outside of the Tracy Vulkan macros, you will have to null-check it. 
    tracy::VkCtx* m_TracyVulkanContext = nullptr;
    
    vk::Queue m_GraphicsQueue;
    vk::Queue m_PresentQueue;
    vk::CommandPool m_CommandPool;
    vk::PhysicalDevice m_PhysicalDevice;
    vk::Device m_LogicalDevice;
};

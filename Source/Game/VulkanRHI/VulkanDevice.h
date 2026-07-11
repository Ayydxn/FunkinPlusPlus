#pragma once

#include "VulkanIncludes.h"

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

class CVulkanDevice
{
public:
    explicit CVulkanDevice(const vk::Instance& VulkanInstance, const vk::SurfaceKHR& ProbeSurface);
    
    void Destroy() const;
    
    const vk::PhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; }
    const vk::Device& GetLogicalDevice() const { return m_LogicalDevice; }
    const vk::Queue& GetGraphicsQueue() const { return m_GraphicsQueue; }
    const vk::Queue& GetPresentQueue() const { return m_PresentQueue; }
    const FVulkanDeviceInfo& GetDeviceInfo() const { return m_DeviceInfo; }
private:
    vk::PhysicalDevice SelectPhysicalDevice(const vk::Instance& VulkanInstance, const vk::SurfaceKHR& ProbeSurface);
    vk::Device CreateLogicalDevice(const vk::PhysicalDevice& PhysicalDevice);
    
    bool IsPhysicalDeviceSuitable(const vk::PhysicalDevice& PhysicalDevice, const vk::SurfaceKHR& ProbeSurface);
    bool DoesPhysicalDeviceSupportRequiredExtensions(const vk::PhysicalDevice& PhysicalDevice);
    
    FQueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice& PhysicalDevice, const vk::SurfaceKHR& ProbeSurface);
    
    // Physical device helpers
    std::string GetVendorNameFromID(uint32 VendorID);
    std::string UnpackDriverVersion(uint32 VendorID, uint32 DriverVersion);
    std::string UnpackVulkanAPIVersion(uint32 VulkanAPIVersion);
private:
    FVulkanDeviceInfo m_DeviceInfo;
    FQueueFamilyIndices m_QueueFamilyIndices;
    
    vk::Queue m_GraphicsQueue;
    vk::Queue m_PresentQueue;
    vk::PhysicalDevice m_PhysicalDevice;
    vk::Device m_LogicalDevice;
};

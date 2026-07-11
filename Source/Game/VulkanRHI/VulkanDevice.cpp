#include "FunkinPCH.h"
#include "VulkanDevice.h"

#include "VulkanDebugUtils.h"

constexpr std::array<const char*, 1> GRequiredPhysicalDeviceExtensions
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

CVulkanDevice::CVulkanDevice(const vk::Instance& VulkanInstance, const vk::SurfaceKHR& ProbeSurface)
{
    /* -- Physical Device Selection -- */
    m_PhysicalDevice = SelectPhysicalDevice(VulkanInstance, ProbeSurface);
    verifyFunkinf(m_PhysicalDevice != VK_NULL_HANDLE, "Failed to select a Vulkan physical device! No suitable GPUs with Vulkan support were found!")
    
    m_QueueFamilyIndices = FindQueueFamilies(m_PhysicalDevice, ProbeSurface);
    verifyFunkinf(m_QueueFamilyIndices.IsComplete(), "Failed to find a suitable graphics queue family on the selected Vulkan physical device!")
    
    m_DeviceInfo = {};
    m_DeviceInfo.PhysicalDeviceProperties = m_PhysicalDevice.getProperties();
    m_DeviceInfo.VendorName = GetVendorNameFromID(m_PhysicalDevice.getProperties().vendorID);
    m_DeviceInfo.DriverVersion = UnpackDriverVersion(m_PhysicalDevice.getProperties().vendorID, m_PhysicalDevice.getProperties().driverVersion);
    m_DeviceInfo.VulkanAPIVersion = UnpackVulkanAPIVersion(m_PhysicalDevice.getProperties().apiVersion);
    
    LOG_INFO_TAG("VulkanRHI", "Graphics Card Information:");
    LOG_INFO_TAG("VulkanRHI", "   Device: {}", m_DeviceInfo.PhysicalDeviceProperties.deviceName.data());
    LOG_INFO_TAG("VulkanRHI", "   Vendor: {}", m_DeviceInfo.VendorName);
    LOG_INFO_TAG("VulkanRHI", "   Driver Version: {}", m_DeviceInfo.DriverVersion);
    LOG_INFO_TAG("VulkanRHI", "   Vulkan API Version: {}", m_DeviceInfo.VulkanAPIVersion);
    
    /* -- Logical Device Creation -- */
    m_LogicalDevice = CreateLogicalDevice(m_PhysicalDevice);

    m_GraphicsQueue = m_LogicalDevice.getQueue(m_QueueFamilyIndices.GraphicsFamily.value(), 0);
    m_PresentQueue = m_LogicalDevice.getQueue(m_QueueFamilyIndices.PresentFamily.value(), 0);
}

void CVulkanDevice::Destroy() const
{
    const vk::Result WaitIdleResult = m_LogicalDevice.waitIdle();
    if (WaitIdleResult != vk::Result::eSuccess)
        LOG_ERROR_TAG("VulkanRHI", "Failed to wait for the Vulkan logical device to idle before shutdown! ({})", vk::to_string(WaitIdleResult));
    
    m_LogicalDevice.destroy();
}

vk::PhysicalDevice CVulkanDevice::SelectPhysicalDevice(const vk::Instance& VulkanInstance, const vk::SurfaceKHR& ProbeSurface)
{
    const auto DeviceEnumerationResult = VulkanInstance.enumeratePhysicalDevices();
    verifyFunkinf(DeviceEnumerationResult.result == vk::Result::eSuccess, "Failed to enumerate Vulkan physical devices! ({})", vk::to_string(DeviceEnumerationResult.result))
    
    const auto AvailablePhysicalDevices = DeviceEnumerationResult.value;
    verifyFunkinf(!AvailablePhysicalDevices.empty(), "Failed to select a Vulkan physical device! No GPUs with Vulkan support were found!")
    
    for (const auto PhysicalDevice : AvailablePhysicalDevices)
    {
        if (IsPhysicalDeviceSuitable(PhysicalDevice, ProbeSurface))
            return PhysicalDevice;
    }
    
    return VK_NULL_HANDLE;
}

vk::Device CVulkanDevice::CreateLogicalDevice(const vk::PhysicalDevice& PhysicalDevice)
{
    constexpr float QueuePriority = 1.0f;
    const std::set<uint32> UniqueQueueFamilies = { m_QueueFamilyIndices.GraphicsFamily.value(), m_QueueFamilyIndices.PresentFamily.value() };
    
    std::vector<vk::DeviceQueueCreateInfo> DeviceQueueCreateInfos;
    DeviceQueueCreateInfos.reserve(UniqueQueueFamilies.size());
    
    for (const uint32 UniqueQueueFamily : UniqueQueueFamilies)
    {
        vk::DeviceQueueCreateInfo DeviceQueueCreateInfo = {};
        DeviceQueueCreateInfo.sType = vk::StructureType::eDeviceQueueCreateInfo;
        DeviceQueueCreateInfo.queueFamilyIndex = UniqueQueueFamily;
        DeviceQueueCreateInfo.queueCount = 1;
        DeviceQueueCreateInfo.pQueuePriorities = &QueuePriority;
        
        DeviceQueueCreateInfos.push_back(DeviceQueueCreateInfo);
    }
    
    vk::PhysicalDeviceVulkan13Features PhysicalDeviceVulkan13Features = {};
    PhysicalDeviceVulkan13Features.sType = vk::StructureType::ePhysicalDeviceVulkan13Features;
    PhysicalDeviceVulkan13Features.dynamicRendering = vk::True;
    PhysicalDeviceVulkan13Features.synchronization2 = vk::True;
    
    vk::DeviceCreateInfo DeviceCreateInfo = {};
    DeviceCreateInfo.sType = vk::StructureType::eDeviceCreateInfo;
    DeviceCreateInfo.queueCreateInfoCount = 1;
    DeviceCreateInfo.pQueueCreateInfos = DeviceQueueCreateInfos.data();
    DeviceCreateInfo.enabledExtensionCount = static_cast<uint32>(GRequiredPhysicalDeviceExtensions.size());
    DeviceCreateInfo.ppEnabledExtensionNames = GRequiredPhysicalDeviceExtensions.data();
    DeviceCreateInfo.pNext = &PhysicalDeviceVulkan13Features;
    DeviceCreateInfo.flags = vk::DeviceCreateFlags();
    
    vk::Device LogicalDevice;
    VK_CHECK_RESULT(PhysicalDevice.createDevice(DeviceCreateInfo), LogicalDevice, "Failed to create Vulkan logical device!")
    
    return LogicalDevice;
}

bool CVulkanDevice::IsPhysicalDeviceSuitable(const vk::PhysicalDevice& PhysicalDevice, const vk::SurfaceKHR& ProbeSurface)
{
    const vk::PhysicalDeviceProperties PhysicalDeviceProperties = PhysicalDevice.getProperties();
    const bool bIsDeviceTypeAppropriate = PhysicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu || PhysicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu;
    const bool bDoesPhysicalDeviceSupportRequiredExtensions = DoesPhysicalDeviceSupportRequiredExtensions(PhysicalDevice);
    const bool bHasSuitableQueueFamilies = FindQueueFamilies(PhysicalDevice, ProbeSurface).IsComplete();
    
    return bIsDeviceTypeAppropriate && bDoesPhysicalDeviceSupportRequiredExtensions && bHasSuitableQueueFamilies;
}

bool CVulkanDevice::DoesPhysicalDeviceSupportRequiredExtensions(const vk::PhysicalDevice& PhysicalDevice)
{
    const auto ExtensionEnumerationResult = PhysicalDevice.enumerateDeviceExtensionProperties();
    if (ExtensionEnumerationResult.result != vk::Result::eSuccess)
    {
        LOG_WARN_TAG("VulkanRHI", "Failed to enumerate device extension properties for a physical device! ({})", vk::to_string(ExtensionEnumerationResult.result));
        return false;
    }
    
    const std::vector<vk::ExtensionProperties>& AvailablePhysicalDeviceExtensions = ExtensionEnumerationResult.value;
    std::set<std::string> RequiredExtensions(GRequiredPhysicalDeviceExtensions.begin(), GRequiredPhysicalDeviceExtensions.end());

    for (const vk::ExtensionProperties& AvailablePhysicalDeviceExtension : AvailablePhysicalDeviceExtensions)
        RequiredExtensions.erase(AvailablePhysicalDeviceExtension.extensionName.data());

    return RequiredExtensions.empty();
}

FQueueFamilyIndices CVulkanDevice::FindQueueFamilies(const vk::PhysicalDevice& PhysicalDevice, const vk::SurfaceKHR& ProbeSurface)
{
    FQueueFamilyIndices QueueFamilyIndices = {};
    
    const std::vector<vk::QueueFamilyProperties> QueueFamilyProperties = PhysicalDevice.getQueueFamilyProperties();
    
    for (uint32 i = 0; i < static_cast<uint32>(QueueFamilyProperties.size()); i++)
    {
        if (QueueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
            QueueFamilyIndices.GraphicsFamily = i;
        
        const auto SurfaceSupportResult = PhysicalDevice.getSurfaceSupportKHR(i, ProbeSurface);
        if (SurfaceSupportResult.result == vk::Result::eSuccess && SurfaceSupportResult.value == vk::True)
            QueueFamilyIndices.PresentFamily = i;
        
        if (QueueFamilyIndices.IsComplete())
            break;
    }
    
    return QueueFamilyIndices;
}

std::string CVulkanDevice::GetVendorNameFromID(uint32 VendorID)
{
    switch (VendorID)
    {
        case 0x1002: return "AMD Incorporated";
        case 0x1010: return "Imagination Technologies Limited";
        case 0x10DE: return "NVIDIA Corporation";
        case 0x13B5: return "ARM";
        case 0x5143: return "Qualcomm";
        case 0x8080: return "Intel Corporation";
        default: return "Unknown GPU Vendor";
    }
}

std::string CVulkanDevice::UnpackDriverVersion(uint32 VendorID, uint32 DriverVersion)
{
    if (VendorID == 0x10DE)
    {
        uint32 DriverMajorVersion = DriverVersion >> 22 & 0x3ff;
        uint32 DriverMinorVersion = DriverVersion >> 14 & 0x0ff;
        uint32 DriverPatchVersion = DriverVersion >> 6 & 0x0ff;
        uint32 DriverVariantVersion = DriverVersion & 0x003f;

        return std::format("{}.{}.{}.{}", DriverMajorVersion, DriverMinorVersion, DriverPatchVersion, DriverVariantVersion);
    }

    #ifdef FUNKIN_PLATFORM_WIN64
        // Intel
        if (VendorID == 0x8086)
        {
            uint32 DriverMajorVersion = DriverVersion >> 14;
            uint32 DriverMinorVersion = DriverVersion & 0x3fff;
            
            return std::format("{}.{}", DriverMajorVersion, DriverMinorVersion);
        }
    #endif

    // Use Vulkan version conventions if a vendor mapping isn't available.
    uint32 DriverMajorVersion = DriverVersion >> 22;
    uint32 DriverMinorVersion = DriverVersion >> 12 & 0x3ff;
    uint32 DriverPatchVersion = DriverVersion & 0xfff;

    return std::format("{}.{}.{}", DriverMajorVersion, DriverMinorVersion, DriverPatchVersion);
}

std::string CVulkanDevice::UnpackVulkanAPIVersion(uint32 VulkanAPIVersion)
{
    return std::format("{}.{}.{}", VK_API_VERSION_MAJOR(VulkanAPIVersion), VK_API_VERSION_MINOR(VulkanAPIVersion), VK_API_VERSION_PATCH(VulkanAPIVersion));
}

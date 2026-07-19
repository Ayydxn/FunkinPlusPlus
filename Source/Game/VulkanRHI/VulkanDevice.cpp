#include "FunkinPCH.h"
#include "VulkanDevice.h"
#include "TracyVulkanAdapter.h"
#include "VulkanDebugUtils.h"

constexpr std::array<const char*, 1> GRequiredPhysicalDeviceExtensions
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

CVulkanDevice::CVulkanDevice(const vk::Instance& VulkanInstance, const vk::SurfaceKHR& ProbeSurface)
{
    /* -- Physical Device Selection -- */
    SelectPhysicalDevice(VulkanInstance, ProbeSurface);
    
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
    CreateLogicalDevice(m_PhysicalDevice);

    m_GraphicsQueue = m_LogicalDevice.getQueue(m_QueueFamilyIndices.GraphicsFamily.value(), 0);
    m_PresentQueue = m_LogicalDevice.getQueue(m_QueueFamilyIndices.PresentFamily.value(), 0);
    
    /* -- Other Resources -- */
    CreateCommandPool();
    InitializeTracyContext(VulkanInstance);
}

void CVulkanDevice::Destroy() const
{
    WaitIdle();
    
    #ifdef TRACY_ENABLE
        if (m_TracyVulkanContext)
            TracyVkDestroy(m_TracyVulkanContext)
    #endif
    
    m_LogicalDevice.destroyCommandPool(m_CommandPool);
    m_LogicalDevice.destroy();
}

void CVulkanDevice::RegisterWindow(uint32 WindowID, uint32 FramesInFlight)
{
    verifyFunkinf(!m_WindowCommandBuffers.contains(WindowID), "Attempted to register window ID {} with the Vulkan device, but it is already registered!", WindowID)
    verifyFunkinf(FramesInFlight > 0, "Attempted to register window ID {} with 0 frames in flight!", WindowID)
    
    vk::CommandBufferAllocateInfo CommandBufferAllocateInfo = {};
    CommandBufferAllocateInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
    CommandBufferAllocateInfo.commandPool = m_CommandPool;
    CommandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
    CommandBufferAllocateInfo.commandBufferCount = FramesInFlight;
    
    std::vector<vk::CommandBuffer> CommandBuffers;
    VK_CHECK_RESULT(m_LogicalDevice.allocateCommandBuffers(CommandBufferAllocateInfo), CommandBuffers, "Failed to allocate Vulkan command buffers for window ID {}", WindowID)
    
    m_WindowCommandBuffers.emplace(WindowID, std::move(CommandBuffers));
}

void CVulkanDevice::UnregisterWindow(uint32 WindowID)
{
    const auto WindowCommandBuffersIterator = m_WindowCommandBuffers.find(WindowID);
    verifyFunkinf(WindowCommandBuffersIterator != m_WindowCommandBuffers.end(), "Attempted to unregister window ID {} with the Vulkan device, but it was never registered!", WindowID)
    
    m_LogicalDevice.freeCommandBuffers(m_CommandPool, WindowCommandBuffersIterator->second);
    m_WindowCommandBuffers.erase(WindowCommandBuffersIterator);
}

void CVulkanDevice::WaitIdle() const
{
    const vk::Result WaitIdleResult = m_LogicalDevice.waitIdle();
    if (WaitIdleResult != vk::Result::eSuccess)
        LOG_ERROR_TAG("VulkanRHI", "Failed to wait for the Vulkan logical device to idle! ({})", vk::to_string(WaitIdleResult));
}

vk::Result CVulkanDevice::Submit(const FSubmitInfo& SubmitInfo) const
{
    vk::SubmitInfo VulkanSubmitInfo = {};
    VulkanSubmitInfo.sType = vk::StructureType::eSubmitInfo;
    VulkanSubmitInfo.waitSemaphoreCount = 1;
    VulkanSubmitInfo.pWaitSemaphores = &SubmitInfo.WaitSemaphore;
    VulkanSubmitInfo.pWaitDstStageMask = &SubmitInfo.WaitStage;
    VulkanSubmitInfo.commandBufferCount = 1;
    VulkanSubmitInfo.pCommandBuffers = &SubmitInfo.CommandBuffer;
    VulkanSubmitInfo.signalSemaphoreCount = 1;
    VulkanSubmitInfo.pSignalSemaphores = &SubmitInfo.SignalSemaphore;
    
    return m_GraphicsQueue.submit(1, &VulkanSubmitInfo, SubmitInfo.SignalFence);
}

vk::Result CVulkanDevice::Present(const FPresentInfo& PresentInfo) const
{
    vk::PresentInfoKHR VulkanPresentInfo = {};
    VulkanPresentInfo.sType = vk::StructureType::ePresentInfoKHR;
    VulkanPresentInfo.waitSemaphoreCount = 1;
    VulkanPresentInfo.pWaitSemaphores = &PresentInfo.WaitSemaphore;
    VulkanPresentInfo.swapchainCount = 1;
    VulkanPresentInfo.pSwapchains = &PresentInfo.SwapChain;
    VulkanPresentInfo.pImageIndices = &PresentInfo.ImageIndex;
    
    return m_PresentQueue.presentKHR(VulkanPresentInfo);
}

vk::CommandBuffer CVulkanDevice::GetCommandBuffer(uint32 WindowID, uint32 FrameIndex) const
{
    const auto WindowCommandBuffersIterator = m_WindowCommandBuffers.find(WindowID);
    verifyFunkinf(WindowCommandBuffersIterator != m_WindowCommandBuffers.end(), "Attempted to get a command buffer for window ID {}, but it is not registered!", WindowID)
    
    const std::vector<vk::CommandBuffer>& CommandBuffers = WindowCommandBuffersIterator->second;
    verifyFunkinf(FrameIndex < CommandBuffers.size(), "Frame index {} is out of range for window ID {} (which has {} frames in flight)!", FrameIndex, WindowID, CommandBuffers.size())
    
    return CommandBuffers[FrameIndex];
}

void CVulkanDevice::SelectPhysicalDevice(const vk::Instance& VulkanInstance, const vk::SurfaceKHR& ProbeSurface)
{
    const auto DeviceEnumerationResult = VulkanInstance.enumeratePhysicalDevices();
    verifyFunkinf(DeviceEnumerationResult.result == vk::Result::eSuccess, "Failed to enumerate Vulkan physical devices! ({})", vk::to_string(DeviceEnumerationResult.result))
    
    const auto AvailablePhysicalDevices = DeviceEnumerationResult.value;
    verifyFunkinf(!AvailablePhysicalDevices.empty(), "Failed to select a Vulkan physical device! No GPUs with Vulkan support were found!")
    
    uint32 SuitablePhysicalDeviceCount = 0;
    uint32 BestPhysicalDeviceScore = 0;
    
    for (const auto PhysicalDevice : AvailablePhysicalDevices)
    {
        if (!IsPhysicalDeviceSuitable(PhysicalDevice, ProbeSurface))
            continue;
        
        SuitablePhysicalDeviceCount++;
        
        const uint32 PhysicalDeviceScore = RatePhysicalDevice(PhysicalDevice);
        if (PhysicalDeviceScore > BestPhysicalDeviceScore)
        {
            m_PhysicalDevice = PhysicalDevice;
            BestPhysicalDeviceScore = PhysicalDeviceScore;
        }
    }
    
    verifyFunkinf(m_PhysicalDevice != VK_NULL_HANDLE, "Failed to select a Vulkan physical device! No suitable GPUs with Vulkan support were found!")
    
    const auto PhysicalDeviceProperties = m_PhysicalDevice.getProperties();
    
    LOG_INFO_TAG("VulkanRHI", "Selected physical device '{}' ({}) as the best candidate out of {} suitable device(s).", PhysicalDeviceProperties.deviceName.data(),
        vk::to_string(PhysicalDeviceProperties.deviceType), SuitablePhysicalDeviceCount);
}

void CVulkanDevice::CreateLogicalDevice(const vk::PhysicalDevice& PhysicalDevice)
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
    
    vk::PhysicalDeviceVulkan11Features PhysicalDeviceVulkan11Features = {};
    PhysicalDeviceVulkan11Features.sType = vk::StructureType::ePhysicalDeviceVulkan11Features;
    PhysicalDeviceVulkan11Features.shaderDrawParameters = vk::True;
    
    vk::PhysicalDeviceVulkan13Features PhysicalDeviceVulkan13Features = {};
    PhysicalDeviceVulkan13Features.sType = vk::StructureType::ePhysicalDeviceVulkan13Features;
    PhysicalDeviceVulkan13Features.dynamicRendering = vk::True;
    PhysicalDeviceVulkan13Features.synchronization2 = vk::True;
    PhysicalDeviceVulkan13Features.pNext = &PhysicalDeviceVulkan11Features;
    
    vk::DeviceCreateInfo DeviceCreateInfo = {};
    DeviceCreateInfo.sType = vk::StructureType::eDeviceCreateInfo;
    DeviceCreateInfo.queueCreateInfoCount = 1;
    DeviceCreateInfo.pQueueCreateInfos = DeviceQueueCreateInfos.data();
    DeviceCreateInfo.enabledExtensionCount = static_cast<uint32>(GRequiredPhysicalDeviceExtensions.size());
    DeviceCreateInfo.ppEnabledExtensionNames = GRequiredPhysicalDeviceExtensions.data();
    DeviceCreateInfo.pNext = &PhysicalDeviceVulkan13Features;
    DeviceCreateInfo.flags = vk::DeviceCreateFlags();
    
    VK_CHECK_RESULT(PhysicalDevice.createDevice(DeviceCreateInfo), m_LogicalDevice, "Failed to create Vulkan logical device!")
}

void CVulkanDevice::CreateCommandPool()
{
    vk::CommandPoolCreateInfo CommandPoolCreateInfo = {};
    CommandPoolCreateInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
    CommandPoolCreateInfo.queueFamilyIndex = m_QueueFamilyIndices.GraphicsFamily.value();
    CommandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    
    VK_CHECK_RESULT(m_LogicalDevice.createCommandPool(CommandPoolCreateInfo), m_CommandPool, "Failed to create Vulkan command pool!")
}

void CVulkanDevice::InitializeTracyContext(const vk::Instance& VulkanInstance)
{
    #ifdef TRACY_ENABLE
        vk::CommandBufferAllocateInfo CommandBufferAllocateInfo = {};
        CommandBufferAllocateInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
        CommandBufferAllocateInfo.commandPool = m_CommandPool;
        CommandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
        CommandBufferAllocateInfo.commandBufferCount = 1;
    
        std::vector<vk::CommandBuffer> InitializationCommandBuffers;
        VK_CHECK_RESULT(m_LogicalDevice.allocateCommandBuffers(CommandBufferAllocateInfo), InitializationCommandBuffers, "Failed to allocate temporary Vulkan command buffer for Tracy!")

        const vk::CommandBuffer InitializationCommandBuffer = InitializationCommandBuffers[0];

        m_TracyVulkanContext = TracyVkContextCalibrated(VulkanInstance, m_PhysicalDevice, m_LogicalDevice, m_GraphicsQueue, InitializationCommandBuffer,
                VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr, VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceProcAddr)
    #endif
}

bool CVulkanDevice::IsPhysicalDeviceSuitable(const vk::PhysicalDevice& PhysicalDevice, const vk::SurfaceKHR& ProbeSurface)
{
    const vk::PhysicalDeviceProperties PhysicalDeviceProperties = PhysicalDevice.getProperties();
    const bool bIsDeviceTypeAppropriate = PhysicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu || PhysicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu;
    const bool bDoesPhysicalDeviceSupportRequiredExtensions = DoesPhysicalDeviceSupportRequiredExtensions(PhysicalDevice);
    const bool bDoesPhysicalDeviceSupportRequiredFeatures = DoesPhysicalDeviceSupportRequiredFeatures(PhysicalDevice);
    const bool bHasSuitableQueueFamilies = FindQueueFamilies(PhysicalDevice, ProbeSurface).IsComplete();
    
    return bIsDeviceTypeAppropriate && bDoesPhysicalDeviceSupportRequiredExtensions && bDoesPhysicalDeviceSupportRequiredFeatures && bHasSuitableQueueFamilies;
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

bool CVulkanDevice::DoesPhysicalDeviceSupportRequiredFeatures(const vk::PhysicalDevice& PhysicalDevice)
{
    vk::PhysicalDeviceVulkan11Features PhysicalDeviceVulkan11Features = {};
    PhysicalDeviceVulkan11Features.sType = vk::StructureType::ePhysicalDeviceVulkan11Features;

    vk::PhysicalDeviceFeatures2 PhysicalDeviceFeatures2 = {};
    PhysicalDeviceFeatures2.sType = vk::StructureType::ePhysicalDeviceFeatures2;
    PhysicalDeviceFeatures2.pNext = &PhysicalDeviceVulkan11Features;

    PhysicalDevice.getFeatures2(&PhysicalDeviceFeatures2);

    return PhysicalDeviceVulkan11Features.shaderDrawParameters == vk::True;
}

uint32 CVulkanDevice::RatePhysicalDevice(const vk::PhysicalDevice& PhysicalDevice)
{
    const vk::PhysicalDeviceProperties PhysicalDeviceProperties = PhysicalDevice.getProperties();
    uint32 Score = 0;
    
    switch (PhysicalDeviceProperties.deviceType)
    {
        case vk::PhysicalDeviceType::eDiscreteGpu: Score += 100; break;
        case vk::PhysicalDeviceType::eIntegratedGpu: Score += 10; break;
        default: break;
    }
    
    return Score;
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
        case 0x8086: return "Intel Corporation";
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

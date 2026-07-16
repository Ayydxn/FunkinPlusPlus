#include "FunkinPCH.h"
#include "VulkanSwapChain.h"
#include "VulkanDebugUtils.h"
#include "VulkanPlatformUtils.h"

CVulkanSwapChain::CVulkanSwapChain(const std::shared_ptr<CVulkanDevice>& VulkanDevice, vk::Instance VulkanInstance, uint32 WindowID, const FNativeWindowHandle& NativeWindowHandle,
                                   const vk::Extent2D& InitialSize, uint32 FramesInFlight, bool bRequestVSync)
        : m_VulkanDevice(VulkanDevice), m_WindowID(WindowID), m_FramesInFlight(FramesInFlight), bRequestVSync(bRequestVSync)
{
    m_Surface = CVulkanPlatformUtils::CreateSurface(VulkanInstance, NativeWindowHandle);
    
    m_VulkanDevice->RegisterWindow(m_WindowID, m_FramesInFlight);
    
    CreateFrameSyncObjects();
    
    // Swapchains created at a (0, 0) size are invalid per the Vulkan specification.
    // So, in the event a swapchain is attempted to be created at that size, it gets left uninitialized.
    // This then defers the creation of the swapchain until the window is resized to a valid size. 
    if (InitialSize.width > 0 && InitialSize.height > 0)
        CreateSwapChainAndDependents(InitialSize, VK_NULL_HANDLE);
}

void CVulkanSwapChain::Destroy(const vk::Instance& VulkanInstance)
{
    DestroySwapChainAndDependents();
    DestroyFrameSyncObjects();
    
    m_VulkanDevice->UnregisterWindow(m_WindowID);
    
    VulkanInstance.destroySurfaceKHR(m_Surface);
}

void CVulkanSwapChain::Resize(uint32 NewWidth, uint32 NewHeight)
{
    // A resize to (0, 0) means the window was minimized.
    // There's nothing to swap in, so just tear down whatever currently exists and leave the swapchain uninitialized, matching the behavior present in the constructor.
    if (NewWidth == 0 || NewHeight == 0)
    {
        DestroySwapChainAndDependents();
        return;
    }
    
    const vk::Device& LogicalDevice = m_VulkanDevice->GetLogicalDevice();
    
    const vk::Result WaitIdleResult = LogicalDevice.waitIdle();
    verifyFunkinf(WaitIdleResult == vk::Result::eSuccess, "Failed to wait for the Vulkan logical device to idle before resizing the swapchain for window ID {}! ({})",
        m_WindowID, vk::to_string(WaitIdleResult))
    
    // Capture the current swapchain and its dependents locally before CreateSwapChainAndDependents() overwrites them with the resources of the new swapchain.
    // This is required for the correct oldSwapchain hand-off pattern: the old swapchain must still be alive when the new one is created,
    // and its own dependents can only be destroyed afterward.
    const vk::SwapchainKHR OldSwapChain = m_SwapChain;
    const std::vector<vk::ImageView> OldSwapChainImageViews = m_SwapChainImageViews;
    const std::vector<vk::Semaphore> OldRenderFinishedSemaphores = m_RenderFinishedSemaphores;
    
    const vk::Extent2D NewExtent = { NewWidth, NewHeight };
    CreateSwapChainAndDependents(NewExtent, OldSwapChain);
    
    // Only now, with the new swapchain fully created, is it safe to destroy the resources of the previous swapchain.
    for (const vk::Semaphore& OldRenderFinishedSemaphore : OldRenderFinishedSemaphores)
        LogicalDevice.destroySemaphore(OldRenderFinishedSemaphore);
    
    for (const vk::ImageView& OldSwapChainImageView : OldSwapChainImageViews)
        LogicalDevice.destroyImageView(OldSwapChainImageView);
    
    if (OldSwapChain)
        LogicalDevice.destroySwapchainKHR(OldSwapChain);
}

FAcquiredFrame CVulkanSwapChain::AcquireNextImage()
{
    const vk::Device& LogicalDevice = m_VulkanDevice->GetLogicalDevice();
    const auto& [ImageAvailableSemaphore, InFlightFence] = m_FrameSyncObjects[m_CurrentFrameIndex];
    
    const vk::Result WaitForFencesResult = LogicalDevice.waitForFences(1, &InFlightFence, vk::True, std::numeric_limits<uint64>::max());
    verifyFunkinf(WaitForFencesResult == vk::Result::eSuccess, "Failed to wait for the in-flight fence for window ID {}! ({})", m_WindowID, vk::to_string(WaitForFencesResult))
    
    uint32 AcquiredImageIndex = 0;
    const vk::Result AcquireResult = LogicalDevice.acquireNextImageKHR(m_SwapChain, std::numeric_limits<uint64>::max(), ImageAvailableSemaphore,VK_NULL_HANDLE,
        &AcquiredImageIndex);
    
    // (Ayydxn) We can recover from eErrorOutOfDateKHR/eSuboptimalKHR, so it's up to the caller to handle that. Typically, it'll just be by calling Resize(). 
    if (AcquireResult != vk::Result::eSuccess && AcquireResult != vk::Result::eSuboptimalKHR)
    {
        return {
            .ImageIndex = 0,
            .FrameIndex = m_CurrentFrameIndex,
            .AcquisitionResult = AcquireResult
        };
    }
    
    const vk::Result ResetFencesResult = LogicalDevice.resetFences(1, &InFlightFence);
    verifyFunkinf(ResetFencesResult == vk::Result::eSuccess, "Failed to reset the in-flight fence for window ID {}! ({})", m_WindowID, vk::to_string(ResetFencesResult))
    
    return {
        .ImageIndex = AcquiredImageIndex,
        .FrameIndex = m_CurrentFrameIndex,
        .AcquisitionResult = AcquireResult
    };
}

void CVulkanSwapChain::AdvanceFrame()
{
    m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_FramesInFlight;
}

void CVulkanSwapChain::CreateSwapChainAndDependents(const vk::Extent2D& RequestedSize, const vk::SwapchainKHR& OldSwapchain)
{
    CreateSwapChain(RequestedSize, OldSwapchain);
    CreateImageViews();
    CreateRenderFinishedSemaphores();
}

void CVulkanSwapChain::DestroySwapChainAndDependents()
{
    if (!m_SwapChain)
        return;
    
    const vk::Device& LogicalDevice = m_VulkanDevice->GetLogicalDevice();
    
    for (const vk::Semaphore& RenderFinishedSemaphore : m_RenderFinishedSemaphores)
        LogicalDevice.destroySemaphore(RenderFinishedSemaphore);
    
    m_RenderFinishedSemaphores.clear();
    
    for (const vk::ImageView& SwapChainImageView : m_SwapChainImageViews)
        LogicalDevice.destroyImageView(SwapChainImageView);
    
    m_SwapChainImageViews.clear();
    m_SwapChainImages.clear();
    
    LogicalDevice.destroySwapchainKHR(m_SwapChain);
    
    m_SwapChain = VK_NULL_HANDLE;
}

void CVulkanSwapChain::CreateSwapChain(const vk::Extent2D& RequestedSize, const vk::SwapchainKHR& OldSwapchain)
{
    const vk::PhysicalDevice& PhysicalDevice = m_VulkanDevice->GetPhysicalDevice();
    const vk::Device& LogicalDevice = m_VulkanDevice->GetLogicalDevice();
    
    const auto SurfaceCapabilitiesResult = PhysicalDevice.getSurfaceCapabilitiesKHR(m_Surface);
    verifyFunkinf(SurfaceCapabilitiesResult.result == vk::Result::eSuccess, "Failed to query Vulkan surface capabilities for window ID {}! ({})", m_WindowID, vk::to_string(SurfaceCapabilitiesResult.result))
    const vk::SurfaceCapabilitiesKHR& SurfaceCapabilities = SurfaceCapabilitiesResult.value;
    
    vk::Format ChosenImageFormat;
    vk::ColorSpaceKHR ChosenColorSpace;
    SelectImageFormatAndColorSpace(ChosenImageFormat, ChosenColorSpace);
    
    const vk::PresentModeKHR ChosenPresentMode = SelectPresentMode();
    
    vk::Extent2D ChosenExtent = RequestedSize;
    if (SurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32>::max())
    {
        ChosenExtent = SurfaceCapabilities.currentExtent;
    }
    else
    {
        ChosenExtent.width = std::clamp(RequestedSize.width, SurfaceCapabilities.minImageExtent.width, SurfaceCapabilities.maxImageExtent.width);
        ChosenExtent.height = std::clamp(RequestedSize.height, SurfaceCapabilities.minImageExtent.height, SurfaceCapabilities.maxImageExtent.height);
    }
    
    uint32 RequestedImageCount = SurfaceCapabilities.minImageCount + 1;
    if (SurfaceCapabilities.maxImageCount > 0 && RequestedImageCount > SurfaceCapabilities.maxImageCount)
        RequestedImageCount = SurfaceCapabilities.maxImageCount;
    
    const FQueueFamilyIndices& QueueFamilyIndices = m_VulkanDevice->GetQueueFamilyIndices();
    const std::array<uint32, 2> QueueFamilyIndicesArray = { QueueFamilyIndices.GraphicsFamily.value(), QueueFamilyIndices.PresentFamily.value() };
    const bool bQueueFamiliesDiffer = QueueFamilyIndices.GraphicsFamily.value() != QueueFamilyIndices.PresentFamily.value();
    
    vk::SwapchainCreateInfoKHR SwapChainCreateInfo = {};
    SwapChainCreateInfo.sType = vk::StructureType::eSwapchainCreateInfoKHR;
    SwapChainCreateInfo.surface = m_Surface;
    SwapChainCreateInfo.minImageCount = RequestedImageCount;
    SwapChainCreateInfo.imageFormat = ChosenImageFormat;
    SwapChainCreateInfo.imageColorSpace = ChosenColorSpace;
    SwapChainCreateInfo.imageExtent = ChosenExtent;
    SwapChainCreateInfo.imageArrayLayers = 1;
    SwapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    SwapChainCreateInfo.imageSharingMode = bQueueFamiliesDiffer ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive;
    SwapChainCreateInfo.queueFamilyIndexCount = bQueueFamiliesDiffer ? static_cast<uint32>(QueueFamilyIndicesArray.size()) : 0;
    SwapChainCreateInfo.pQueueFamilyIndices = bQueueFamiliesDiffer ? QueueFamilyIndicesArray.data() : nullptr;
    SwapChainCreateInfo.preTransform = SurfaceCapabilities.currentTransform;
    SwapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    SwapChainCreateInfo.presentMode = ChosenPresentMode;
    SwapChainCreateInfo.clipped = vk::True;
    SwapChainCreateInfo.oldSwapchain = OldSwapchain;
    SwapChainCreateInfo.flags = vk::SwapchainCreateFlagsKHR();
    
    VK_CHECK_RESULT(LogicalDevice.createSwapchainKHR(SwapChainCreateInfo), m_SwapChain, "Failed to create Vulkan swapchain for window ID {}!", m_WindowID)
    
    const auto SwapChainImagesResult = LogicalDevice.getSwapchainImagesKHR(m_SwapChain);
    verifyFunkinf(SwapChainImagesResult.result == vk::Result::eSuccess, "Failed to retrieve Vulkan swapchain images for window ID {}!", m_WindowID)
    m_SwapChainImages = SwapChainImagesResult.value;
    
    m_SwapChainImageFormat = ChosenImageFormat;
    m_SwapChainExtent = ChosenExtent;
    
    LOG_DEBUG_TAG("VulkanRHI", "Created Vulkan swapchain for window ID {}:", m_WindowID);
    LOG_DEBUG_TAG("VulkanRHI", " - Image Format: {}", vk::to_string(m_SwapChainImageFormat));
    LOG_DEBUG_TAG("VulkanRHI", " - Color Space: {}", vk::to_string(ChosenColorSpace));
    LOG_DEBUG_TAG("VulkanRHI", " - Present Mode: {}", vk::to_string(ChosenPresentMode));
    LOG_DEBUG_TAG("VulkanRHI", " - Extent: {}x{}", m_SwapChainExtent.width, m_SwapChainExtent.height);
    LOG_DEBUG_TAG("VulkanRHI", " - Image Count: {}", m_SwapChainImages.size());
}

void CVulkanSwapChain::CreateImageViews()
{
    const vk::Device& LogicalDevice = m_VulkanDevice->GetLogicalDevice();
    
    m_SwapChainImageViews.resize(m_SwapChainImages.size());
    
    for (size_t i = 0; i < m_SwapChainImages.size(); i++)
    {
        vk::ImageSubresourceRange ImageSubresourceRange;
        ImageSubresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        ImageSubresourceRange.baseMipLevel = static_cast<uint32>(0);
        ImageSubresourceRange.levelCount = static_cast<uint32>(1);
        ImageSubresourceRange.baseArrayLayer = static_cast<uint32>(0);
        ImageSubresourceRange.layerCount = static_cast<uint32>(1);
        
        vk::ImageViewCreateInfo ImageViewCreateInfo = {};
        ImageViewCreateInfo.sType = vk::StructureType::eImageViewCreateInfo;
        ImageViewCreateInfo.image = m_SwapChainImages[i];
        ImageViewCreateInfo.viewType = vk::ImageViewType::e2D;
        ImageViewCreateInfo.format = m_SwapChainImageFormat;
        ImageViewCreateInfo.components = vk::ComponentMapping();
        ImageViewCreateInfo.subresourceRange = ImageSubresourceRange;
        ImageViewCreateInfo.flags = vk::ImageViewCreateFlags();
        
        VK_CHECK_RESULT(LogicalDevice.createImageView(ImageViewCreateInfo), m_SwapChainImageViews[i], "Failed to create Vulkan swapchain image view {} for window ID {}!", i, m_WindowID)
    }
}

void CVulkanSwapChain::CreateRenderFinishedSemaphores()
{
    const vk::Device& LogicalDevice = m_VulkanDevice->GetLogicalDevice();
    
    m_RenderFinishedSemaphores.resize(m_SwapChainImages.size());
    
    vk::SemaphoreCreateInfo SemaphoreCreateInfo;
    SemaphoreCreateInfo.sType = vk::StructureType::eSemaphoreCreateInfo;
    SemaphoreCreateInfo.pNext = nullptr;
    SemaphoreCreateInfo.flags = vk::SemaphoreCreateFlags();
    
    for (size_t i = 0; i < m_SwapChainImages.size(); i++)
        VK_CHECK_RESULT(LogicalDevice.createSemaphore(SemaphoreCreateInfo), m_RenderFinishedSemaphores[i], "Failed to create Vulkan render finished semaphore for window ID {}!", m_WindowID)
}

void CVulkanSwapChain::CreateFrameSyncObjects()
{
    const vk::Device& LogicalDevice = m_VulkanDevice->GetLogicalDevice();
    
    m_FrameSyncObjects.resize(m_FramesInFlight);
    
    vk::SemaphoreCreateInfo SemaphoreCreateInfo;
    SemaphoreCreateInfo.sType = vk::StructureType::eSemaphoreCreateInfo;
    SemaphoreCreateInfo.pNext = nullptr;
    SemaphoreCreateInfo.flags = vk::SemaphoreCreateFlags();
    
    vk::FenceCreateInfo FenceCreateInfo;
    FenceCreateInfo.sType = vk::StructureType::eFenceCreateInfo;
    FenceCreateInfo.pNext = nullptr;
    FenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;
    
    for (uint32 i = 0; i < m_FramesInFlight; i++)
    {
        VK_CHECK_RESULT(LogicalDevice.createSemaphore(SemaphoreCreateInfo), m_FrameSyncObjects[i].ImageAvailableSemaphore, "Failed to create Vulkan image-available semaphore {} for window ID {}!", i, m_WindowID)
        VK_CHECK_RESULT(LogicalDevice.createFence(FenceCreateInfo), m_FrameSyncObjects[i].InFlightFence, "Failed to create Vulkan in-flight fence {} for window ID {}!", i, m_WindowID)
    }
}

void CVulkanSwapChain::DestroyFrameSyncObjects()
{
    const vk::Device& LogicalDevice = m_VulkanDevice->GetLogicalDevice();
    
    for (const auto& [ImageAvailableSemaphore, InFlightFence] : m_FrameSyncObjects)
    {
        LogicalDevice.destroySemaphore(ImageAvailableSemaphore);
        LogicalDevice.destroyFence(InFlightFence);
    }
    
    m_FrameSyncObjects.clear();
}

void CVulkanSwapChain::SelectImageFormatAndColorSpace(vk::Format& OutImageFormat, vk::ColorSpaceKHR& OutColorSpace)
{
    const vk::PhysicalDevice& PhysicalDevice = m_VulkanDevice->GetPhysicalDevice();
    
    const auto SurfaceFormatsResult = PhysicalDevice.getSurfaceFormatsKHR(m_Surface);
    verifyFunkinf(SurfaceFormatsResult.result == vk::Result::eSuccess && !SurfaceFormatsResult.value.empty(), "Failed to query Vulkan surface formats for window ID {}!", m_WindowID)
    
    const std::vector<vk::SurfaceFormatKHR>& SurfaceFormats = SurfaceFormatsResult.value;
    
    bool bFoundPreferredFormat = false;
    for (const vk::SurfaceFormatKHR& SurfaceFormat : SurfaceFormats)
    {
        // If we find VK_FORMAT_B8G8R8A8_SRGB, we use that and its associated color space.
        if (SurfaceFormat.format == vk::Format::eB8G8R8A8Srgb && SurfaceFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            OutImageFormat = SurfaceFormat.format;
            OutColorSpace = SurfaceFormat.colorSpace;
            bFoundPreferredFormat = true;
            break;
        }
    }
    
    // If we failed to find VK_FORMAT_B8G8R8A8_SRGB, fall back to the format/color space of the first available surface format.
    if (!bFoundPreferredFormat)
    {
        OutImageFormat = SurfaceFormats[0].format;
        OutColorSpace = SurfaceFormats[0].colorSpace;
    }
}

vk::PresentModeKHR CVulkanSwapChain::SelectPresentMode()
{
    const vk::PhysicalDevice& PhysicalDevice = m_VulkanDevice->GetPhysicalDevice();
    
    const auto PresentModesResult = PhysicalDevice.getSurfacePresentModesKHR(m_Surface);
    verifyFunkinf(PresentModesResult.result == vk::Result::eSuccess && !PresentModesResult.value.empty(), "Failed to query Vulkan surface present modes for window ID {}!", m_WindowID)
    
    const std::vector<vk::PresentModeKHR>& AvailablePresentModes = PresentModesResult.value;
    
    // FIFO is guaranteed by the Vulkan specification to always be available, so no fallback search is actually needed here.
    // However, we still verify its presence explicitly rather than assume to catch a non-conformant driver/platform rather than silently misbehave.
    if (bRequestVSync)
    {
        const bool bIsFIFOAvailable = std::ranges::find(AvailablePresentModes, vk::PresentModeKHR::eFifo) != AvailablePresentModes.end();
        verifyFunkinf(bIsFIFOAvailable, "VK_PRESENT_MODE_FIFO_KHR is unexpectedly unavailable for window ID {}! This present mode is required to be supported per the Vulkan specification.", m_WindowID)
        
        return vk::PresentModeKHR::eFifo;
    }
    
    // If we don't want VSync, prefer Mailbox (low latency, no tearing), fall back to Immediate (tearing allowed), fall back to FIFO if neither is available.
    const bool bIsMailboxAvailable = std::ranges::find(AvailablePresentModes, vk::PresentModeKHR::eMailbox) != AvailablePresentModes.end();
    if (bIsMailboxAvailable)
        return vk::PresentModeKHR::eMailbox;
    
    const bool bIsImmediateAvailable = std::ranges::find(AvailablePresentModes, vk::PresentModeKHR::eImmediate) != AvailablePresentModes.end();
    if (bIsImmediateAvailable)
        return vk::PresentModeKHR::eImmediate;
    
    return vk::PresentModeKHR::eFifo;
}

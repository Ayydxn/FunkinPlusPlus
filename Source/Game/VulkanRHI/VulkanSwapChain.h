#pragma once

#include "VulkanDevice.h"
#include "Application/Window.h"

struct FFrameSyncObjects
{
    vk::Semaphore ImageAvailableSemaphore;
    vk::Fence InFlightFence;
};

class CVulkanSwapChain
{
public:
    CVulkanSwapChain(const std::shared_ptr<CVulkanDevice>& VulkanDevice, vk::Instance VulkanInstance, uint32 WindowID, const FNativeWindowHandle& NativeWindowHandle,
        const vk::Extent2D& InitialSize, uint32 FramesInFlight, bool bRequestVSync);
    
    void Destroy(const vk::Instance& VulkanInstance);
    void Resize(uint32 NewWidth, uint32 NewHeight);
    
    bool IsValid() const { return static_cast<bool>(m_SwapChain); }

    vk::Format GetImageFormat() const { return m_SwapChainImageFormat; }
    const vk::Extent2D& GetExtent() const { return m_SwapChainExtent; }
    uint32 GetWindowID() const { return m_WindowID; }
private:
    void CreateSwapChainAndDependents(const vk::Extent2D& RequestedSize, const vk::SwapchainKHR& OldSwapchain);
    void DestroySwapChainAndDependents();
    
    void CreateSwapChain(const vk::Extent2D& RequestedSize, const vk::SwapchainKHR& OldSwapchain);
    void CreateImageViews();
    void CreateRenderFinishedSemaphores();
    
    void CreateFrameSyncObjects();
    void DestroyFrameSyncObjects();
    
    void SelectImageFormatAndColorSpace(vk::Format& OutImageFormat, vk::ColorSpaceKHR& OutColorSpace);
    vk::PresentModeKHR SelectPresentMode();
private:
    std::vector<vk::Image> m_SwapChainImages;
    std::vector<vk::ImageView> m_SwapChainImageViews;
    std::vector<vk::Semaphore> m_RenderFinishedSemaphores;
    std::vector<FFrameSyncObjects> m_FrameSyncObjects;
    
    std::shared_ptr<CVulkanDevice> m_VulkanDevice;
    
    vk::SurfaceKHR m_Surface;
    vk::Format m_SwapChainImageFormat;
    vk::Extent2D m_SwapChainExtent;
    vk::SwapchainKHR m_SwapChain;
    
    uint32 m_WindowID = 0;
    uint32 m_FramesInFlight = 0;
    uint32 m_CurrentFrameIndex = 0;
    
    bool bRequestVSync;
};

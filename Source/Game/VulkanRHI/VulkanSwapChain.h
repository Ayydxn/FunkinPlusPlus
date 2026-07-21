#pragma once

#include "VulkanDevice.h"
#include "Application/Window.h"

struct FFrameSyncObjects
{
    vk::Semaphore ImageAvailableSemaphore;
    vk::Fence InFlightFence;
};

struct FAcquiredFrame
{
    uint32 ImageIndex;
    uint32 FrameIndex;
    vk::Result AcquisitionResult;
};

class CVulkanSwapChain
{
public:
    CVulkanSwapChain(CVulkanDevice& VulkanDevice, vk::Instance VulkanInstance,const FNativeWindowHandle& NativeWindowHandle,
        const vk::Extent2D& InitialSize, uint32 FramesInFlight, bool bRequestVSync);
    
    void Destroy(const vk::Instance& VulkanInstance);
    void Resize(uint32 NewWidth, uint32 NewHeight);
    
    FAcquiredFrame AcquireNextImage();
    void AdvanceFrame();
    
    bool IsValid() const { return static_cast<bool>(m_SwapChain); }
    
    vk::CommandBuffer GetCommandBuffer(uint32 FrameIndex) const { return m_VulkanDevice.GetCommandBuffer(FrameIndex); }
    vk::Image GetImage(uint32 ImageIndex) const { return m_SwapChainImages[ImageIndex]; }
    vk::ImageView GetImageView(uint32 ImageIndex) const { return m_SwapChainImageViews[ImageIndex]; }
    vk::Semaphore GetRenderFinishedSemaphore(uint32 ImageIndex) const { return m_RenderFinishedSemaphores[ImageIndex]; }
    vk::Semaphore GetImageAvailableSemaphore(uint32 FrameIndex) const { return m_FrameSyncObjects[FrameIndex].ImageAvailableSemaphore; }
    vk::Fence GetInFlightFence(uint32 FrameIndex) const { return m_FrameSyncObjects[FrameIndex].InFlightFence; }
    vk::Format GetImageFormat() const { return m_SwapChainImageFormat; }
    const vk::Extent2D& GetExtent() const { return m_SwapChainExtent; }
    vk::SwapchainKHR GetHandle() const { return m_SwapChain; }
private:
    void CreateSwapChainAndDependents(const vk::Extent2D& RequestedSize, vk::SwapchainKHR OldSwapchain);
    void DestroySwapChainAndDependents();
    
    void CreateSwapChain(const vk::Extent2D& RequestedSize, vk::SwapchainKHR OldSwapchain);
    void CreateImageViews();
    void CreateRenderFinishedSemaphores();
    
    void CreateFrameSyncObjects();
    void DestroyFrameSyncObjects();
    
    void SelectImageFormatAndColorSpace(vk::Format& OutImageFormat, vk::ColorSpaceKHR& OutColorSpace) const;
    vk::PresentModeKHR SelectPresentMode() const;
private:
    std::vector<vk::Image> m_SwapChainImages;
    std::vector<vk::ImageView> m_SwapChainImageViews;
    std::vector<vk::Semaphore> m_RenderFinishedSemaphores;
    std::vector<FFrameSyncObjects> m_FrameSyncObjects;
    
    CVulkanDevice& m_VulkanDevice;
    
    vk::PhysicalDevice m_PhysicalDevice;
    vk::Device m_LogicalDevice;
    vk::SurfaceKHR m_Surface;
    vk::Format m_SwapChainImageFormat;
    vk::Extent2D m_SwapChainExtent;
    vk::SwapchainKHR m_SwapChain;
    
    uint32 m_FramesInFlight = 0;
    uint32 m_CurrentFrameIndex = 0;
    
    bool bRequestVSync;
};

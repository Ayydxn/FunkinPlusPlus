#include "FunkinPCH.h"
#include "VulkanDynamicRHI.h"
#include "VulkanDebugUtils.h"
#include "VulkanUtils.h"

CVulkanDynamicRHI::CVulkanDynamicRHI(CVulkanContext& VulkanContext)
    : m_VulkanContext(VulkanContext) {}

void CVulkanDynamicRHI::BeginFrame()
{
    for (uint32 WindowID : m_VulkanContext.GetOrderedWindowIDs())
    {
        CVulkanSwapChain* SwapChain = m_VulkanContext.GetSwapChain(WindowID);
        if (!SwapChain && !SwapChain->IsValid())
            return;
    
        const FAcquiredFrame AcquiredFrame = SwapChain->AcquireNextImage();
        if (AcquiredFrame.AcquisitionResult == vk::Result::eErrorOutOfDateKHR)
        {
            const vk::Extent2D& CurrentExtent = SwapChain->GetExtent();
            SwapChain->Resize(CurrentExtent.width, CurrentExtent.height);
        
            return;
        }
    
        verifyFunkinf(AcquiredFrame.AcquisitionResult == vk::Result::eSuccess || AcquiredFrame.AcquisitionResult == vk::Result::eSuboptimalKHR,
            "Failed to acquire the next swapchain image for window ID {}! ({})", SwapChain->GetWindowID(), vk::to_string(AcquiredFrame.AcquisitionResult))
    
        m_AcquiredFramesThisFrame[WindowID] = AcquiredFrame;
    
        const vk::CommandBuffer CommandBuffer = SwapChain->GetCommandBuffer(AcquiredFrame.FrameIndex);
        VK_CHECK_RESULT_VOID(CommandBuffer.reset(), "Failed to reset Vulkan command buffer for window ID {}!", SwapChain->GetWindowID())
    
        vk::CommandBufferBeginInfo CommandBufferBeginInfo = {};
        CommandBufferBeginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
        CommandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    
        VK_CHECK_RESULT_VOID(CommandBuffer.begin(CommandBufferBeginInfo), "Failed to begin recording a Vulkan command buffer for window ID {}!", SwapChain->GetWindowID())
    
        const vk::Image AcquiredImage = SwapChain->GetImage(AcquiredFrame.ImageIndex);
    
        CVulkanUtils::TransitionImageLayout(CommandBuffer, AcquiredImage, vk::PipelineStageFlagBits2::eTopOfPipe,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::AccessFlagBits2::eNone,
            vk::AccessFlagBits2::eColorAttachmentWrite, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal);
    
        vk::ClearColorValue ClearColorValue;
        ClearColorValue.float32 = std::array<float, 4> { 0.05f, 0.05f, 0.05f, 1.0f };
    
        vk::RenderingAttachmentInfo ColorAttachmentInfo = {};
        ColorAttachmentInfo.sType = vk::StructureType::eRenderingAttachmentInfo;
        ColorAttachmentInfo.imageView = SwapChain->GetImageView(AcquiredFrame.ImageIndex);
        ColorAttachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        ColorAttachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
        ColorAttachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
        ColorAttachmentInfo.clearValue = ClearColorValue;
    
        vk::RenderingInfo RenderingInfo = {};
        RenderingInfo.sType = vk::StructureType::eRenderingInfo;
        RenderingInfo.renderArea = vk::Rect2D({ 0, 0 }, SwapChain->GetExtent());
        RenderingInfo.layerCount = 1;
        RenderingInfo.colorAttachmentCount = 1;
        RenderingInfo.pColorAttachments = &ColorAttachmentInfo;
    
        CommandBuffer.beginRendering(RenderingInfo);
    }
}

void CVulkanDynamicRHI::EndFrame()
{
    for (uint32 WindowID : m_VulkanContext.GetOrderedWindowIDs())
    {
        CVulkanSwapChain* SwapChain = m_VulkanContext.GetSwapChain(WindowID);
        
        const auto AcquiredFrameIterator = m_AcquiredFramesThisFrame.find(WindowID);
        if (!SwapChain || !SwapChain->IsValid() || AcquiredFrameIterator == m_AcquiredFramesThisFrame.end())
            return;
    
        const FAcquiredFrame AcquiredFrame = AcquiredFrameIterator->second;
    
        const vk::CommandBuffer CommandBuffer = SwapChain->GetCommandBuffer(AcquiredFrame.FrameIndex);
        CommandBuffer.endRendering();
    
        const vk::Image AcquiredImage = SwapChain->GetImage(AcquiredFrame.ImageIndex);
    
        CVulkanUtils::TransitionImageLayout(CommandBuffer, AcquiredImage, vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::PipelineStageFlagBits2::eBottomOfPipe, vk::AccessFlagBits2::eColorAttachmentWrite,
            vk::AccessFlagBits2::eNone, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR);
    
        VK_CHECK_RESULT_VOID(CommandBuffer.end(), "Failed to end recording a Vulkan command buffer for window ID {}!", SwapChain->GetWindowID())

        const CVulkanDevice& VulkanDevice = m_VulkanContext.GetDevice();
    
        FSubmitInfo SubmitInfo;
        SubmitInfo.CommandBuffer = CommandBuffer;
        SubmitInfo.WaitSemaphore = SwapChain->GetImageAvailableSemaphore(AcquiredFrame.FrameIndex);
        SubmitInfo.WaitStage = vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        SubmitInfo.SignalSemaphore = SwapChain->GetRenderFinishedSemaphore(AcquiredFrame.ImageIndex);
        SubmitInfo.SignalFence = SwapChain->GetInFlightFence(AcquiredFrame.FrameIndex);
    
        const vk::Result SubmitResult = VulkanDevice.Submit(SubmitInfo);
        verifyFunkinf(SubmitResult == vk::Result::eSuccess, "Failed to submit the Vulkan command buffer for window ID {}! ({})", SwapChain->GetWindowID(),
            vk::to_string(SubmitResult))
    
        FPresentInfo PresentInfo;
        PresentInfo.SwapChain = SwapChain->GetHandle();
        PresentInfo.ImageIndex = AcquiredFrame.ImageIndex;
        PresentInfo.WaitSemaphore = SwapChain->GetRenderFinishedSemaphore(AcquiredFrame.ImageIndex);
    
        const vk::Result PresentResult = VulkanDevice.Present(PresentInfo);
        if (PresentResult == vk::Result::eErrorOutOfDateKHR || PresentResult == vk::Result::eSuboptimalKHR)
        {
            const vk::Extent2D& CurrentExtent = SwapChain->GetExtent();
            SwapChain->Resize(CurrentExtent.width, CurrentExtent.height);
        }
        else
        {
            verifyFunkinf(PresentResult == vk::Result::eSuccess, "Failed to present the Vulkan swapchain for window ID {}! ({})", SwapChain->GetWindowID(),
                vk::to_string(PresentResult))
        }
    
        SwapChain->AdvanceFrame();
    }
}

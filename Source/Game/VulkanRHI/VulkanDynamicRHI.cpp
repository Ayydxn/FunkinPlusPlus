#include "FunkinPCH.h"
#include "VulkanDynamicRHI.h"
#include "TracyVulkanAdapter.h"
#include "VulkanDebugUtils.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanUtils.h"

CVulkanDynamicRHI::CVulkanDynamicRHI(CVulkanContext& VulkanContext)
    : m_VulkanContext(VulkanContext) {}

bool CVulkanDynamicRHI::BeginFrame()
{
    const  std::shared_ptr<CVulkanSwapChain> SwapChain = m_VulkanContext.GetSwapChain();
    if (!SwapChain || !SwapChain->IsValid())
        return false;
    
    const FAcquiredFrame AcquiredFrame = SwapChain->AcquireNextImage();
    if (AcquiredFrame.AcquisitionResult == vk::Result::eErrorOutOfDateKHR)
    {
        const vk::Extent2D& CurrentExtent = SwapChain->GetExtent();
        SwapChain->Resize(CurrentExtent.width, CurrentExtent.height);
    
        return false;
    }
    
    verifyFunkinf(AcquiredFrame.AcquisitionResult == vk::Result::eSuccess || AcquiredFrame.AcquisitionResult == vk::Result::eSuboptimalKHR,
        "Failed to acquire the next swapchain image! ({})",vk::to_string(AcquiredFrame.AcquisitionResult))
    
    m_CurrentlyAcquiredFrame = AcquiredFrame;
    
    const vk::CommandBuffer CommandBuffer = SwapChain->GetCommandBuffer(AcquiredFrame.FrameIndex);
    VK_CHECK_RESULT_VOID(CommandBuffer.reset(), "Failed to reset the Vulkan command buffer!")
    
    vk::CommandBufferBeginInfo CommandBufferBeginInfo = {};
    CommandBufferBeginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
    CommandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    
    VK_CHECK_RESULT_VOID(CommandBuffer.begin(CommandBufferBeginInfo), "Failed to begin recording a Vulkan command buffer!")
    
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
    
    FUNKIN_PROFILE_VULKAN_ZONE(m_VulkanContext.GetDevice().GetTracyContext(), CommandBuffer, __FUNCTION__)
    
    CommandBuffer.beginRendering(RenderingInfo);
    
    return true;
}

void CVulkanDynamicRHI::EndFrame()
{
    const std::shared_ptr<CVulkanSwapChain> SwapChain = m_VulkanContext.GetSwapChain();
    if (!SwapChain || !SwapChain->IsValid() || !m_CurrentlyAcquiredFrame.has_value())
        return;
    
    const FAcquiredFrame AcquiredFrame = m_CurrentlyAcquiredFrame.value();
    
    const vk::CommandBuffer CommandBuffer = SwapChain->GetCommandBuffer(AcquiredFrame.FrameIndex);
    CommandBuffer.endRendering();
    
    const vk::Image AcquiredImage = SwapChain->GetImage(AcquiredFrame.ImageIndex);
    
    CVulkanUtils::TransitionImageLayout(CommandBuffer, AcquiredImage, vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eBottomOfPipe, vk::AccessFlagBits2::eColorAttachmentWrite,
        vk::AccessFlagBits2::eNone, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR);
    
    FUNKIN_PROFILE_VULKAN_COLLECT(m_VulkanContext.GetDevice().GetTracyContext(), CommandBuffer)
    
    VK_CHECK_RESULT_VOID(CommandBuffer.end(), "Failed to stop recording a Vulkan command buffer!")

    const CVulkanDevice& VulkanDevice = m_VulkanContext.GetDevice();
    
    FSubmitInfo SubmitInfo;
    SubmitInfo.CommandBuffer = CommandBuffer;
    SubmitInfo.WaitSemaphore = SwapChain->GetImageAvailableSemaphore(AcquiredFrame.FrameIndex);
    SubmitInfo.WaitStage = vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    SubmitInfo.SignalSemaphore = SwapChain->GetRenderFinishedSemaphore(AcquiredFrame.ImageIndex);
    SubmitInfo.SignalFence = SwapChain->GetInFlightFence(AcquiredFrame.FrameIndex);
    
    const vk::Result SubmitResult = VulkanDevice.Submit(SubmitInfo);
    verifyFunkinf(SubmitResult == vk::Result::eSuccess, "Failed to submit the Vulkan command buffer! ({})", vk::to_string(SubmitResult))
    
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
        verifyFunkinf(PresentResult == vk::Result::eSuccess, "Failed to present the Vulkan swapchain! ({})", vk::to_string(PresentResult))
    }
    
    m_CurrentlyAcquiredFrame.reset();
    
    SwapChain->AdvanceFrame();
}

void CVulkanDynamicRHI::BindPipeline(const IGraphicsPipeline& GraphicsPipeline)
{
    const std::shared_ptr<CVulkanSwapChain> SwapChain = m_VulkanContext.GetSwapChain();
    if (!SwapChain || !SwapChain->IsValid() || !m_CurrentlyAcquiredFrame.has_value())
        return;
    
    const FAcquiredFrame AcquiredFrame = m_CurrentlyAcquiredFrame.value();
    const vk::CommandBuffer CommandBuffer = SwapChain->GetCommandBuffer(AcquiredFrame.FrameIndex);
    
    const auto& VulkanPipeline = dynamic_cast<const CVulkanGraphicsPipeline&>(GraphicsPipeline);
    
    const vk::Extent2D& SwapChainExtent = m_VulkanContext.GetSwapChain()->GetExtent();
    
    vk::Viewport Viewport;
    Viewport.x = 0.0f;
    Viewport.y = 0.0f;
    Viewport.width = static_cast<float>(SwapChainExtent.width);
    Viewport.height = static_cast<float>(SwapChainExtent.height);
    Viewport.minDepth = 0.0f;
    Viewport.maxDepth = 1.0f;
    
    FUNKIN_PROFILE_VULKAN_ZONE(m_VulkanContext.GetDevice().GetTracyContext(), CommandBuffer, __FUNCTION__)
    
    CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, VulkanPipeline.GetHandle());
    CommandBuffer.setViewport(0, Viewport);
    CommandBuffer.setScissor(0, vk::Rect2D({ 0, 0 }, SwapChainExtent));
}

void CVulkanDynamicRHI::Draw(uint32 VertexCount, uint32 InstanceCount)
{
    const std::shared_ptr<CVulkanSwapChain> SwapChain = m_VulkanContext.GetSwapChain();
    if (!SwapChain || !SwapChain->IsValid() || !m_CurrentlyAcquiredFrame.has_value())
        return;
    
    const FAcquiredFrame AcquiredFrame = m_CurrentlyAcquiredFrame.value();
    const vk::CommandBuffer CommandBuffer = SwapChain->GetCommandBuffer(AcquiredFrame.FrameIndex);
    
    FUNKIN_PROFILE_VULKAN_ZONE(m_VulkanContext.GetDevice().GetTracyContext(), CommandBuffer, __FUNCTION__)
    
    CommandBuffer.draw(VertexCount, InstanceCount, 0, 0);
}

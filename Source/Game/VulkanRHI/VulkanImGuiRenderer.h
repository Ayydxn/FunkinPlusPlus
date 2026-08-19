#pragma once

#include "RHICore/ImGuiRenderer.h"
#include "VulkanContext.h"

class CVulkanImGuiRenderer final : public IImGuiRenderer
{
public:
    CVulkanImGuiRenderer(CVulkanContext& VulkanContext);
    ~CVulkanImGuiRenderer() override = default;
    
    void Initialize(const FNativeWindowHandle& NativeWindowHandle) override;
    void Shutdown() override;
    
    void BeginFrame() override;
    void EndFrame(ICommandBuffer* CommandBuffer) override;
private:
    void CreateDescriptorPool();
private:
    CVulkanContext& m_VulkanContext;
    
    vk::DescriptorPool m_DescriptorPool;
};

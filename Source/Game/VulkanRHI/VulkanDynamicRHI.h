#pragma once

#include "VulkanContext.h"
#include "RHICore/DynamicRHI.h"

class CVulkanDynamicRHI final : public IDynamicRHI
{
public:
    explicit CVulkanDynamicRHI(CVulkanContext& VulkanContext);
    ~CVulkanDynamicRHI() override = default;
    
    bool BeginFrame() override;
    void EndFrame() override;
    
    void BindPipeline(const IGraphicsPipeline& GraphicsPipeline) override;
    void BindVertexBuffer(const IVertexBuffer& VertexBuffer) override;
    void BindIndexBuffer(const IIndexBuffer& IndexBuffer) override;
    
    void Draw(uint32 VertexCount, uint32 InstanceCount) override;
    void DrawIndexed(uint32 IndexCount, uint32 InstanceCount) override;
    
    void* GetCurrentCommandBuffer() const override;
private:
    std::optional<FAcquiredFrame> m_CurrentlyAcquiredFrame = std::nullopt;
    
    CVulkanContext& m_VulkanContext;
};

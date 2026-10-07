#pragma once

#include "VulkanCommandBuffer.h"
#include "VulkanContext.h"
#include "VulkanGraphicsPipeline.h"
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
    void BindUniformBuffer(uint32 Set, uint32 Binding, const IUniformBuffer& UniformBuffer) override;
    
    void Draw(uint32 VertexCount, uint32 InstanceCount, uint32 FirstInstance) override;
    void DrawIndexed(uint32 IndexCount, uint32 InstanceCount) override;
    
    ICommandBuffer* GetCurrentCommandBuffer() const override;
    uint32 GetCurrentFrameIndex() const override;
private:
    CVulkanCommandBuffer* GetCurrentVulkanCommandBuffer() const;
private:
    std::optional<FAcquiredFrame> m_CurrentlyAcquiredFrame = std::nullopt;
    mutable std::optional<CVulkanCommandBuffer> m_CurrentCommandBuffer = std::nullopt;
    
    const CVulkanGraphicsPipeline* m_CurrentlyBoundPipeline = nullptr;
    
    CVulkanContext& m_VulkanContext;
};

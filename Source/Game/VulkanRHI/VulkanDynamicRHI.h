#pragma once

#include "VulkanContext.h"
#include "RHICore/DynamicRHI.h"

class CVulkanDynamicRHI final : public IDynamicRHI
{
public:
    explicit CVulkanDynamicRHI(CVulkanContext& VulkanContext);
    ~CVulkanDynamicRHI() override = default;
    
    bool BeginFrame(uint32 WindowID) override;
    void EndFrame(uint32 WindowID) override;
    
    void BindPipeline(uint32 WindowID, const IGraphicsPipeline& GraphicsPipeline) override;
    void Draw(uint32 WindowID, uint32 VertexCount, uint32 InstanceCount) override;
private:
    std::unordered_map<uint32, FAcquiredFrame> m_AcquiredFramesThisFrame;
    
    CVulkanContext& m_VulkanContext;
};

#pragma once

#include "VulkanContext.h"
#include "RHICore/DynamicRHI.h"

class CVulkanDynamicRHI final : public IDynamicRHI
{
public:
    explicit CVulkanDynamicRHI(CVulkanContext& VulkanContext);
    ~CVulkanDynamicRHI() override = default;
    
    void BeginFrame() override;
    void EndFrame() override;
private:
    std::unordered_map<uint32, FAcquiredFrame> m_AcquiredFramesThisFrame;
    
    CVulkanContext& m_VulkanContext;
};

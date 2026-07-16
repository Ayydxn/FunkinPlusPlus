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
    CVulkanContext& m_VulkanContext;
    std::optional<FAcquiredFrame> m_CurrentAcquiredFrame;
};

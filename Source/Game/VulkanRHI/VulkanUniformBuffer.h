#pragma once

#include "VulkanContext.h"
#include "RHICore/UniformBuffer.h"

class CVulkanUniformBuffer : public IUniformBuffer
{
public:
    CVulkanUniformBuffer(CVulkanContext& VulkanContext, const FUniformBufferDescription& Description);
    ~CVulkanUniformBuffer() override;
    
    void SetData(const void* Data, uint64 SizeInBytes) override;
    
    vk::Buffer GetHandle() const { return m_AllocatedUniformBuffer.Buffer; }
private:
    CVulkanContext& m_VulkanContext;
    FAllocatedVulkanBuffer m_AllocatedUniformBuffer;
};

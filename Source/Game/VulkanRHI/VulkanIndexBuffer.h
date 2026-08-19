#pragma once

#include "VulkanContext.h"
#include "RHICore/IndexBuffer.h"

class CVulkanIndexBuffer final : public IIndexBuffer
{
public:
    CVulkanIndexBuffer(CVulkanContext& VulkanContext, const FIndexBufferDescription& Description);
    ~CVulkanIndexBuffer() override;
    
    void SetData(const void* Data, uint64 SizeInBytes) override;
    
    uint32 GetCount() override { return m_Size / sizeof(uint32); }
    
    vk::Buffer GetHandle() const { return m_AllocatedIndexBuffer.Buffer; }
private:
    CVulkanContext& m_VulkanContext;
    
    FAllocatedVulkanBuffer m_AllocatedIndexBuffer;
    uint32 m_Size = 0;
};

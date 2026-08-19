#pragma once

#include "VulkanContext.h"
#include "RHICore/VertexBuffer.h"

class CVulkanVertexBuffer final : public IVertexBuffer
{
public:
    CVulkanVertexBuffer(CVulkanContext& VulkanContext, const FVertexBufferDescription& Description);
    ~CVulkanVertexBuffer() override;
    
    void SetData(const void* Data, uint64 SizeInBytes) override;
    
    vk::Buffer GetHandle() const { return m_AllocatedVertexBuffer.Buffer; }
private:
    void UploadDataViaStaging(const void* Data, uint64 SizeInBytes) const;
private:
    CVulkanContext& m_VulkanContext;
    
    FAllocatedVulkanBuffer m_AllocatedVertexBuffer;
};

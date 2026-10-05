#pragma once

#include "RHICore/RHIContext.h"

struct FUniformBufferDescription
{
    uint64 SizeInBytes;
    const void* InitialData = nullptr;
};

class IUniformBuffer
{
public:
    virtual ~IUniformBuffer() = default;

    IUniformBuffer(const IUniformBuffer&) = delete;
    IUniformBuffer& operator=(const IUniformBuffer&) = delete;
    
    virtual void SetData(const void* Data, uint64 SizeInBytes) = 0;
    
    uint64 GetSizeInBytes() const { return m_SizeInBytes; }
protected:
    IUniformBuffer() = default;
protected:
    uint64 m_SizeInBytes = 0;
};

std::shared_ptr<IUniformBuffer> CreateUniformBuffer(ERHIBackend RHIBackend, IRHIContext& RHIContext, const FUniformBufferDescription& Description);

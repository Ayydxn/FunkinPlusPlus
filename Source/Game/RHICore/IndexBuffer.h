#pragma once

#include "RHICore/RHIContext.h"

struct FIndexBufferDescription
{
    uint64 SizeInBytes = 0;
    
    // Optional initial data, uploaded during creation via the same path SetData() uses later.
    // Leave null to create an empty/uninitialized buffer of SizeInBytes (typical for dynamic buffers you intend to write to post-construction).
    const void* InitialData = nullptr;
};

class IIndexBuffer
{
public:
    virtual ~IIndexBuffer() = default;
    
    // Data must fit within the buffer's existing size (SizeInBytes at creation) - this writes into the existing allocation, it doesn't resize.
    // If you wanna grow it, create a new buffer instead.
    virtual void SetData(const void* Data, uint64 SizeInBytes) = 0;
    
    virtual uint32 GetCount() = 0;
protected:
    IIndexBuffer() = default;
};

std::shared_ptr<IIndexBuffer> CreateIndexBuffer(ERHIBackend RHIBackend, IRHIContext& RHIContext, const FIndexBufferDescription& Description);

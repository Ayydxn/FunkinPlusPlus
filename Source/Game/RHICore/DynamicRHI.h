#pragma once

#include "RHIContext.h"

class IDynamicRHI
{
public:
    virtual ~IDynamicRHI() = default;
    
    IDynamicRHI(const IDynamicRHI&) = delete;
    IDynamicRHI& operator=(const IDynamicRHI&) = delete;
    
    virtual bool BeginFrame(uint32 WindowID) = 0;
    virtual void EndFrame(uint32 WindowID) = 0;
protected:
    IDynamicRHI() = default;
};

std::unique_ptr<IDynamicRHI> CreateDynamicRHI(ERHIBackend RHIBackend, IRHIContext& RHIContext);
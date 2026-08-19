#pragma once

#include "CommandBuffer.h"
#include "RHIContext.h"

class IImGuiRenderer
{
public:
    virtual ~IImGuiRenderer() = default;
    
    IImGuiRenderer(const IImGuiRenderer&) = delete;
    IImGuiRenderer& operator=(const IImGuiRenderer&) = delete;
    
    virtual void Initialize(const FNativeWindowHandle& NativeWindowHandle) = 0;
    virtual void Shutdown() = 0;
    
    virtual void BeginFrame() = 0;
    virtual void EndFrame(ICommandBuffer* CommandBuffer) = 0;
protected:
    IImGuiRenderer() = default;
};

IImGuiRenderer* CreateImGuiRenderer(ERHIBackend RHIBackend, IRHIContext& RHIContext);

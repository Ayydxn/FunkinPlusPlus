#pragma once
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
    virtual void EndFrame(void* CommandBuffer) = 0; // TODO: (Ayydxn) Introduce a concrete RHICore command buffer type.
protected:
    IImGuiRenderer() = default;
};

IImGuiRenderer* CreateImGuiRenderer(ERHIBackend RHIBackend, IRHIContext& RHIContext);

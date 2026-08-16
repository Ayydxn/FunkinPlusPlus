#pragma once
#include "RHIContext.h"

class IImGuiRenderer
{
public:
    virtual ~IImGuiRenderer() = default;
    
    virtual void Initialize(const FNativeWindowHandle& NativeWindowHandle) = 0;
    virtual void Shutdown() = 0;
    
    virtual void BeginFrame() = 0;
    virtual void EndFrame(void* CommandBuffer) = 0; // TODO: (Ayydxn) Introduce a concrete RHICore command buffer type.
};

IImGuiRenderer* CreateImGuiRenderer(ERHIBackend RHIBackend, IRHIContext& RHIContext);

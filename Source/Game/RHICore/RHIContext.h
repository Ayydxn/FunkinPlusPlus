#pragma once

#include "Application/Window.h"

#include <memory>

enum class ERHIBackend
{
    OpenGL,
    Vulkan,
    Direct3D11,
    Direct3D12,
    Metal
};

class IRHIContext
{
public:
    virtual ~IRHIContext() = default;
    
    IRHIContext(const IRHIContext&) = delete;
    IRHIContext& operator=(const IRHIContext&) = delete;
    
    virtual bool Initialize(const FNativeWindowHandle& NativeWindowHandle, uint32 InitialWindowWidth, uint32 InitialWindowHeight, bool bRequestVSync) = 0;
    virtual void Destroy() = 0;
    
    virtual void OnWindowResized(uint32 NewWidth, uint32 NewHeight) = 0;
protected:
    IRHIContext() = default;
};

std::unique_ptr<IRHIContext> CreateRHIContext(ERHIBackend RHIBackend);
std::string GetRHIBackendName(ERHIBackend RHIBackend);

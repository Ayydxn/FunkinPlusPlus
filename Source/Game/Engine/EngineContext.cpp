#include "FunkinPCH.h"
#include "EngineContext.h"

bool CEngineContext::Initialize(ERHIBackend RHIBackend, uint32 WindowID, const FNativeWindowHandle& NativeWindowHandle, uint32 InitialWindowWidth, uint32 InitialWindowHeight,
        bool bRequestVSync)
{
    m_InputState.Initialize(m_EventBroadcaster);
    
    m_RHIContext = CreateRHIContext(RHIBackend);
    if (!m_RHIContext->Initialize(WindowID, NativeWindowHandle, InitialWindowWidth, InitialWindowHeight, bRequestVSync))
        return false;
    
    m_DynamicRHI = CreateDynamicRHI(RHIBackend, *m_RHIContext);
    
    m_Renderer = std::make_unique<CRenderer>(*m_DynamicRHI);
    
    return true;
}

void CEngineContext::Shutdown()
{
    m_Renderer.reset();
    m_DynamicRHI.reset();
    
    m_RHIContext->Destroy();
    m_RHIContext.reset();
    
    m_InputState.Shutdown(m_EventBroadcaster);
}

bool CEngineContext::RegisterWindow(uint32 WindowID, const FNativeWindowHandle& NativeWindowHandle, uint32 InitialWidth, uint32 InitialHeight, bool bRequestVSync) const
{
    return m_RHIContext->RegisterWindow(WindowID, NativeWindowHandle, InitialWidth, InitialHeight, bRequestVSync);
}

void CEngineContext::UnregisterWindow(uint32 WindowID) const
{
    m_RHIContext->UnregisterWindow(WindowID);
}

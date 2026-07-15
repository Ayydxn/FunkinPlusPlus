#include "FunkinPCH.h"
#include "EngineContext.h"

bool CEngineContext::Initialize(ERHIBackend RHIBackend, uint32 WindowID, const FNativeWindowHandle& NativeWindowHandle, uint32 InitialWindowWidth, uint32 InitialWindowHeight,
        bool bRequestVSync)
{
    m_InputState.Initialize(m_EventBroadcaster);
    
    m_RHIContext = CreateRHIContext(RHIBackend);
    if (!m_RHIContext->Initialize(WindowID, NativeWindowHandle, InitialWindowWidth, InitialWindowHeight, bRequestVSync))
        return false;
    
    m_Renderer = std::make_unique<CRenderer>(*m_RHIContext);
    
    return true;
}

void CEngineContext::Shutdown()
{
    m_RHIContext->Destroy();
    m_RHIContext.reset();
    
    m_InputState.Shutdown(m_EventBroadcaster);
}

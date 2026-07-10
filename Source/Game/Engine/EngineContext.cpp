#include "FunkinPCH.h"
#include "EngineContext.h"

bool CEngineContext::Initialize(ERHIBackend RHIBackend, const FNativeWindowHandle& NativeWindowHandle)
{
    m_InputState.Initialize(m_EventBroadcaster);
    
    m_RHIContext = CreateRHIContext(RHIBackend);
    if (!m_RHIContext->Initialize(NativeWindowHandle))
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

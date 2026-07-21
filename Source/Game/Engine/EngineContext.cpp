#include "FunkinPCH.h"
#include "EngineContext.h"

bool CEngineContext::Initialize(ERHIBackend RHIBackend, const FNativeWindowHandle& NativeWindowHandle, uint32 InitialWindowWidth, uint32 InitialWindowHeight, bool bRequestVSync)
{
    m_Instance = this;
    
    m_InputState.Initialize(m_EventBroadcaster);
    
    m_RHIContext = CreateRHIContext(RHIBackend);
    if (!m_RHIContext->Initialize(NativeWindowHandle, InitialWindowWidth, InitialWindowHeight, bRequestVSync))
        return false;
    
    m_DynamicRHI = CreateDynamicRHI(RHIBackend, *m_RHIContext);
    
    m_Renderer = std::make_unique<CRenderer>(*m_DynamicRHI, RHIBackend, *m_RHIContext);
    m_Renderer->Initialize();
    
    return true;
}

void CEngineContext::Shutdown()
{
    m_Instance = nullptr;
    
    m_Renderer.reset();
    m_DynamicRHI.reset();
    
    m_RHIContext->Destroy();
    m_RHIContext.reset();
    
    m_InputState.Shutdown(m_EventBroadcaster);
}

CEngineContext& CEngineContext::GetInstance()
{
    verifyFunkinf(m_Instance, "Attempted to get an instance of CEngineContext before one was available!")
    
    return *m_Instance;
}

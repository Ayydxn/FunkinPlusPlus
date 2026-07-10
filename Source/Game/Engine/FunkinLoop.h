#pragma once

#include "EngineContext.h"
#include "FramePacer.h"
#include "Application/Application.h"
#include "Events/Event.h"

class CFunkinLoop
{
public:
    bool PreInitialize();
    bool Initialize();
    void Tick();
    void Shutdown();
public:
    static constexpr double TickRate = 240.0;
private:
    void OnEvent(IEvent& Event);
    
    FWindowSpecification BuildWindowSpecification() const;
    
    double ResolveDefaultRenderIntervalSeconds() const;
    ERHIBackend ResolveRHIBackend() const;
private:
    CEngineContext m_EngineContext;
    CApplication m_Application;
    CFramePacer m_FramePacer;

    CEventBroadcaster::FListenerHandle m_ListenerHandle = 0;

    bool bIsRunning = false;
};

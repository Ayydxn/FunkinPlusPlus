#pragma once

#include "EngineContext.h"
#include "Application/Application.h"
#include "Events/Event.h"

class CFunkinLoop
{
public:
    bool PreInitialize();
    bool Initialize();
    void Tick();
    void Shutdown();
private:
    void OnEvent(IEvent& Event);
    
    FWindowSpecification BuildWindowSpecification() const;
private:
    CEngineContext m_EngineContext;
    CApplication m_Application;

    CEventBroadcaster::FListenerHandle m_ListenerHandle = 0;

    bool bIsRunning = false;
};

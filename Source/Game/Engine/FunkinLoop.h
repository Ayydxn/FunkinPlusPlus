#pragma once

#include "EngineContext.h"
#include "Application/PlatformApplication.h"
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
private:
    CEngineContext m_EngineContext;
    FPlatformApplication m_Application;

    CEventBroadcaster::FListenerHandle m_ListenerHandle = 0;

    bool bIsRunning = false;
};

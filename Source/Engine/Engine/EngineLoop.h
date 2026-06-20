#pragma once

#include "Application/Generic/GenericApplication.h"
#include "Core/CoreDefines.h"
#include "Core/Events/EventBroadcaster.h"

class FUNKIN_API CFunkinEngineLoop
{
public:
    explicit CFunkinEngineLoop(IGenericApplication* Application);
    
    bool PreInitialize();
    bool Initialize();
    void Tick();
    void Shutdown() const;
private:
    void OnEvent(IEvent& Event);
private:
    IGenericApplication* m_Application = nullptr;
    CEventBroadcaster::FListenerHandle m_ListenerHandle = 0;
    
    bool bIsRunning = false;
};

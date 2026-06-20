#pragma once

#include "Core/Delegates/MulticastDelegate.h"
#include "Core/Events/EventBroadcaster.h"

class FUNKIN_API CFunkinEngine
{
public:
    CFunkinEngine();
    ~CFunkinEngine();
    
    CFunkinEngine(const CFunkinEngine&) = delete;
    CFunkinEngine& operator=(const CFunkinEngine&) = delete;
    
    CEventBroadcaster& GetEventBroadcaster() { return m_MessageBroadcaster; }
private:
    CEventBroadcaster m_MessageBroadcaster;
};

struct FUNKIN_API FEngineDelegates
{
    inline static TMulticastDelegate<> PreInitializeDelegate;
    inline static TMulticastDelegate<> InitializeDelegate;
    inline static TMulticastDelegate<> ShutdownDelegate;
};

extern FUNKIN_API CFunkinEngine* GFunkinEngine;

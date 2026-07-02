#pragma once

#pragma once

#include "Delegates/MulticastDelegate.h"
#include "Events/EventBroadcaster.h"

class CEngineContext
{
public:
    CEngineContext() = default;

    CEngineContext(const CEngineContext&) = delete;
    CEngineContext& operator=(const CEngineContext&) = delete;

    CEventBroadcaster& GetEventBroadcaster() { return m_EventBroadcaster; }
private:
    CEventBroadcaster m_EventBroadcaster;
};

struct FEngineDelegates
{
    inline static TMulticastDelegate<> PreInitializeDelegate;
    inline static TMulticastDelegate<> InitializeDelegate;
    inline static TMulticastDelegate<> ShutdownDelegate;
};

#pragma once
#include "Event.h"

class CEventBroadcaster
{
public:
    using FListenerFunction = std::function<void(IEvent&)>;
    using FListenerHandle = uint32;
    
    // Priority controls dispatch order — lower values fire first.
    // For example, UI capturing input would want a lower value (e.g., -100) to consume events before the gameplay input handler (e.g., 0) ever sees them.
    FListenerHandle AddListener(const FListenerFunction& ListenerFunction, int32 Priority);
    
    void RemoveListener(FListenerHandle ListenerHandle);
    void Broadcast(IEvent& Event) const;
private:
    struct FEntry
    {
        FListenerHandle Handle;
        FListenerFunction ListenerFunction;
        int32 Priority;
    };
    
    std::vector<FEntry> m_Listeners;
    FListenerHandle m_NextHandle = 0;
};

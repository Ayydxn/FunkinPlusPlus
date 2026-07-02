#include "FunkinPCH.h"
#include "EventBroadcaster.h"

CEventBroadcaster::FListenerHandle CEventBroadcaster::AddListener(const FListenerFunction& ListenerFunction, int32 Priority)
{
    const FListenerHandle Handle = m_NextHandle++;
    m_Listeners.emplace_back(Handle, ListenerFunction, Priority);

    std::ranges::stable_sort(m_Listeners, [](const FEntry& A, const FEntry& B) { return A.Priority < B.Priority; });

    return Handle;
}

void CEventBroadcaster::RemoveListener(FListenerHandle ListenerHandle)
{
    std::erase_if(m_Listeners, [ListenerHandle](const FEntry& Entry) { return Entry.Handle == ListenerHandle; });
}

void CEventBroadcaster::Broadcast(IEvent& Event) const
{
    for (const FEntry& Entry : m_Listeners)
    {
        Entry.ListenerFunction(Event);
        
        if (Event.bIsHandled)
            break;
    }
}

#pragma once

#include "Core/CoreDefines.h"

#include <functional>
#include <vector>

template<typename... Args>
class FUNKIN_API TMulticastDelegate
{
public:
    using FDelegateFunction = std::function<void(Args...)>;
    using FDelegateHandle = uint32;
    
    ~TMulticastDelegate()
    {
        m_Listeners.clear();
    }

    FDelegateHandle SubscribeListener(const FDelegateFunction& DelegateFunction)
    {
        const FDelegateHandle Handle = m_NextHandle++;
        m_Listeners.emplace_back(Handle, DelegateFunction);
    
        return Handle;
    }

    void UnsubscribeListener(FDelegateHandle Handle)
    {
        m_Listeners.erase(std::remove_if(m_Listeners.begin(), m_Listeners.end(), [Handle](const FListenerEntry& Entry) { return Entry.Handle == Handle; }), m_Listeners.end());
    }

    void Broadcast(Args... Arguments) const
    {
        for (const std::vector<FListenerEntry> ListenersSnapshot = m_Listeners; const FListenerEntry& Entry : ListenersSnapshot)
            Entry.ListenerFunction(Arguments...);
    }
private:
    struct FListenerEntry
    {
        FDelegateHandle Handle;
        FDelegateFunction ListenerFunction;
    };
    
    std::vector<FListenerEntry> m_Listeners;
    FDelegateHandle m_NextHandle = 0;
};

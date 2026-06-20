#include "FunkinPCH.h"
#include "MulticastDelegate.h"
#include <algorithm>

// Explicit instantiation for zero-argument delegates
template class FUNKIN_API TMulticastDelegate<>;

template<typename ... Args>
TMulticastDelegate<Args...>::~TMulticastDelegate()
{
    for (const FListenerEntry& Entry : m_Listeners)
        UnsubscribeListener(Entry.Handle);
}

template<typename ... Args>
TMulticastDelegate<Args...>::FDelegateHandle TMulticastDelegate<Args...>::SubscribeListener(const FDelegateFunction& DelegateFunction)
{
    const FDelegateHandle Handle = m_NextHandle++;
    m_Listeners.emplace_back(Handle, DelegateFunction);
    
    return Handle;
}

template<typename ... Args>
void TMulticastDelegate<Args...>::UnsubscribeListener(FDelegateHandle Handle)
{
    m_Listeners.erase(std::remove_if(m_Listeners.begin(), m_Listeners.end(), [Handle](const FListenerEntry& Entry) { return Entry.Handle == Handle; }), m_Listeners.end());
}

template<typename ... Args>
void TMulticastDelegate<Args...>::Broadcast(Args... Arguments) const
{
    for (const FListenerEntry& Entry : m_Listeners)
        Entry.ListenerFunction(Arguments...);
}

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
    
    ~TMulticastDelegate();
    
    FDelegateHandle SubscribeListener(const FDelegateFunction& DelegateFunction);
    void UnsubscribeListener(FDelegateHandle Handle);
    
    void Broadcast(Args... Arguments) const;
private:
    struct FListenerEntry
    {
        FDelegateHandle Handle;
        FDelegateFunction ListenerFunction;
    };
    
    std::vector<FListenerEntry> m_Listeners;
    FDelegateHandle m_NextHandle = 0;
};

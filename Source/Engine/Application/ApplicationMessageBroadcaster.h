#pragma once

#include "Generic/GenericApplicationMessageHandler.h"

#include <algorithm>
#include <vector>

class FUNKIN_API CApplicationMessageBroadcaster final : public IGenericApplicationMessageHandler
{
public:
    // Priority controls dispatch order — lower values fire first.
    // For example, UI capturing input would want a lower value (e.g., -100) to consume events before the gameplay input handler (e.g., 0) ever sees them.
    void AddHandler(IGenericApplicationMessageHandler* MessageHandler, int32 Priority = 0);
    
    void RemoveHandler(IGenericApplicationMessageHandler* MessageHandler);
    
    EMessageReply OnExitRequested() override;
private:
    struct FUNKIN_API FListenerEntry
    {
        IGenericApplicationMessageHandler* MessageHandler = nullptr;
        int Priority = 0;
    };

    std::vector<FListenerEntry> m_Listeners;
};

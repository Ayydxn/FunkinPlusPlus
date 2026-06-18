#include "FunkinPCH.h"
#include "ApplicationMessageBroadcaster.h"

void CApplicationMessageBroadcaster::AddHandler(IGenericApplicationMessageHandler* MessageHandler, int32 Priority)
{
    m_Listeners.emplace_back(MessageHandler, Priority);
    
    // Keep sorted by priority ascending so low values fire first.
    std::ranges::stable_sort(m_Listeners, [](const FListenerEntry& A, const FListenerEntry& B)
    {
        return A.Priority < B.Priority;
    });
}

void CApplicationMessageBroadcaster::RemoveHandler(IGenericApplicationMessageHandler* MessageHandler)
{
    std::erase_if(m_Listeners, [MessageHandler](const FListenerEntry& Entry) { return Entry.MessageHandler == MessageHandler; });
}

EMessageReply CApplicationMessageBroadcaster::OnExitRequested()
{
    for (const auto& [Handler, Priority] : m_Listeners)
    {
        if (Handler->OnExitRequested() == EMessageReply::Consumed)
            return EMessageReply::Consumed;
    }
    
    return EMessageReply::Unhandled;
}

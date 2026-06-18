#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"

enum class EMessageReply : uint8
{
    Unhandled, // Pass the message onto the next listener
    Consumed // Don't propagate the message further
};

class FUNKIN_API IGenericApplicationMessageHandler
{
public:
    virtual ~IGenericApplicationMessageHandler() = default;
    
    virtual EMessageReply OnExitRequested() = 0;
};
#pragma once

#include "Application/ApplicationMessageBroadcaster.h"

class FUNKIN_API CFunkinEngine
{
public:
    CFunkinEngine();
    ~CFunkinEngine();
    
    CFunkinEngine(const CFunkinEngine&) = delete;
    CFunkinEngine& operator=(const CFunkinEngine&) = delete;
    
    CApplicationMessageBroadcaster& GetMessageBroadcaster() { return m_MessageBroadcaster; }
private:
    CApplicationMessageBroadcaster m_MessageBroadcaster;
};

extern CFunkinEngine* GFunkinEngine;

#pragma once

#include "Engine/EngineContext.h"

class CDesktopApplication
{
public:
    bool Initialize(CEngineContext& EngineContext);
    void Shutdown();
    
    void PumpMessages();
private:
    CEngineContext* m_EngineContext = nullptr;
};

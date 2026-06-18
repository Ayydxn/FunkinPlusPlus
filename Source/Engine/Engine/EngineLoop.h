#pragma once

#include "Application/Generic/GenericApplication.h"
#include "Core/CoreDefines.h"

class FUNKIN_API CFunkinEngineLoop : public IGenericApplicationMessageHandler
{
public:
    explicit CFunkinEngineLoop(IGenericApplication* Application);
    
    bool PreInitialize();
    bool Initialize();
    void Tick();
    void Shutdown() const;
    
    EMessageReply OnExitRequested() override;
private:
    IGenericApplication* m_Application = nullptr;
    
    bool bIsRunning = false;
};

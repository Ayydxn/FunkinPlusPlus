#include "FunkinPCH.h"
#include "EngineLoop.h"
#include "FunkinEngine.h"

CFunkinEngineLoop::CFunkinEngineLoop(IGenericApplication* Application)
    : m_Application(Application) {}

bool CFunkinEngineLoop::PreInitialize()
{
    return true;
}

bool CFunkinEngineLoop::Initialize()
{
    GFunkinEngine = new CFunkinEngine();
    
    m_Application->SetMessageHandler(&GFunkinEngine->GetMessageBroadcaster());
    
    if (!m_Application->Initialize())
    {
        printf("Application failed to initialize!\n");
        return false;
    }
    
    bIsRunning = true;
    
    return true;
}

void CFunkinEngineLoop::Tick()
{
    while (bIsRunning)
    {
        // PumpMessages will return false from any message that wants us to quit the application.
        if (!m_Application->PumpMessages()) 
        {
            bIsRunning = false;
            break;
        }
        
        /**
         * TODO: 1. Get delta time
         *       2. Perform a game tick (update game state and other things of that nature)
         *       3. Render
         */
    }
}

void CFunkinEngineLoop::Shutdown() const
{
    m_Application->Shutdown();
    
    delete GFunkinEngine;
}

EMessageReply CFunkinEngineLoop::OnExitRequested()
{
    bIsRunning = false;
    
    return EMessageReply::Consumed;
}

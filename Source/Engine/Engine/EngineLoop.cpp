#include "FunkinPCH.h"
#include "EngineLoop.h"
#include "FunkinEngine.h"
#include "Core/Logging/Logging.h"

CFunkinEngineLoop::CFunkinEngineLoop(IGenericApplication* Application)
    : m_Application(Application) {}

bool CFunkinEngineLoop::PreInitialize()
{
    CLogging::Initialize();
    
    return true;
}

bool CFunkinEngineLoop::Initialize()
{
    ENGINE_LOG_INFO_TAG("Core", "Initializing Funkin Engine...");
    
    GFunkinEngine = new CFunkinEngine();
    
    m_Application->SetMessageHandler(&GFunkinEngine->GetMessageBroadcaster());
    
    if (!m_Application->Initialize())
    {
        ENGINE_LOG_CRITICAL_TAG("Core", "Game application failed to initialize!");
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
    ENGINE_LOG_INFO_TAG("Core", "Shutting down...");
    
    m_Application->Shutdown();
    
    CLogging::Shutdown();
    
    delete GFunkinEngine;
}

EMessageReply CFunkinEngineLoop::OnExitRequested()
{
    bIsRunning = false;
    
    return EMessageReply::Consumed;
}

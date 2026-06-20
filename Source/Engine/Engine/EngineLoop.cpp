#include "FunkinPCH.h"
#include "EngineLoop.h"
#include "FunkinEngine.h"
#include "Core/Events/KeyboardEvents.h"
#include "Core/Events/WindowEvents.h"
#include "Core/Logging/Logging.h"

CFunkinEngineLoop::CFunkinEngineLoop(IGenericApplication* Application)
    : m_Application(Application) {}

bool CFunkinEngineLoop::PreInitialize()
{
    CLogging::Initialize();
    
    FEngineDelegates::PreInitializeDelegate.Broadcast();
    
    return true;
}

bool CFunkinEngineLoop::Initialize()
{
    ENGINE_LOG_INFO_TAG("Core", "Initializing Funkin Engine...");
    
    GFunkinEngine = new CFunkinEngine();
    
    if (!m_Application->Initialize())
    {
        ENGINE_LOG_CRITICAL_TAG("Core", "Game application failed to initialize!");
        return false;
    }
    
    m_ListenerHandle = GFunkinEngine->GetEventBroadcaster().AddListener([this](IEvent& Event) { OnEvent(Event); }, 0);
    
    FEngineDelegates::InitializeDelegate.Broadcast();
    
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
    
    FEngineDelegates::ShutdownDelegate.Broadcast();
    
    GFunkinEngine->GetEventBroadcaster().RemoveListener(m_ListenerHandle);
    
    m_Application->Shutdown();
    
    CLogging::Shutdown();

    delete GFunkinEngine;
}

void CFunkinEngineLoop::OnEvent(IEvent& Event)
{
    CEventDispatcher Dispatcher(Event);
    Dispatcher.Dispatch<CWindowCloseEvent>([this](CWindowCloseEvent&)
    {
        bIsRunning = false;
        
        return true; 
    });
}

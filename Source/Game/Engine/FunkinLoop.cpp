#include "FunkinPCH.h"
#include "FunkinLoop.h"
#include "Events/WindowEvents.h"
#include "Logging/Logging.h"

bool CFunkinLoop::PreInitialize()
{
    CLogging::Initialize();
    
    FEngineDelegates::PreInitializeDelegate.Broadcast();
    
    return true;
}

bool CFunkinLoop::Initialize()
{
    LOG_INFO_TAG("Core", "Starting Friday Night Funkin++...");
    
    if (!m_Application.Initialize(m_EngineContext))
        return false;
    
    m_ListenerHandle = m_EngineContext.GetEventBroadcaster().AddListener([this](IEvent& Event) { OnEvent(Event); }, 0);
    
    FEngineDelegates::InitializeDelegate.Broadcast();
    
    bIsRunning = true;
    
    return true;
}

void CFunkinLoop::Tick()
{
    while (bIsRunning)
    {
        m_Application.PumpMessages();
    }
}

void CFunkinLoop::Shutdown()
{
    FEngineDelegates::ShutdownDelegate.Broadcast();
    
    m_EngineContext.GetEventBroadcaster().RemoveListener(m_ListenerHandle);
    
    m_Application.Shutdown();
    
    CLogging::Shutdown();
}

void CFunkinLoop::OnEvent(IEvent& Event)
{
    CEventDispatcher Dispatcher(Event);
    Dispatcher.Dispatch<CWindowCloseEvent>([this](CWindowCloseEvent&)
    {
        bIsRunning = false;
        
        return true; 
    });
}

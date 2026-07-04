#include "FunkinPCH.h"
#include "FunkinLoop.h"
#include "Debug/Profiler.h"
#include "Events/WindowEvents.h"
#include "Logging/Logging.h"
#include "Misc/CommandLine.h"

bool CFunkinLoop::PreInitialize()
{
    CLogging::Initialize();
    
    FEngineDelegates::PreInitializeDelegate.Broadcast();
    
    return true;
}

bool CFunkinLoop::Initialize()
{
    LOG_INFO_TAG("Core", "Starting Friday Night Funkin++...");
    
    m_EngineContext.Initialize();
    
    if (!m_Application.Initialize(m_EngineContext, BuildWindowSpecification()))
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
        FUNKIN_PROFILE_SCOPE(__FUNCTION__)
        
        m_EngineContext.GetInputState().BeginFrame();
        
        m_Application.PumpMessages();
        
        m_EngineContext.GetInputState().EndFrame();
        
        // TODO: (Ayydxn) Once we actually have "rendering" and "presenting", move this call there.
        FUNKIN_PROFILE_MARK_FRAME;
    }
}

void CFunkinLoop::Shutdown()
{
    LOG_INFO_TAG("Core", "Shutting down...");
    
    FEngineDelegates::ShutdownDelegate.Broadcast();
    
    m_EngineContext.GetEventBroadcaster().RemoveListener(m_ListenerHandle);
    m_EngineContext.Shutdown();
    
    m_Application.Shutdown();
    
    CLogging::Shutdown();
}

void CFunkinLoop::OnEvent(IEvent& Event)
{
    CEventDispatcher Dispatcher(Event);
    Dispatcher.Dispatch<CWindowCloseEvent>([this](const CWindowCloseEvent& WindowCloseEvent)
    {
        if (WindowCloseEvent.GetSourceWindowID() == m_Application.GetMainWindow().GetNativeWindowID())
        {
            bIsRunning = false;
            return true;
        }
        
        // CWindowCloseEvent should never hold a CWindow object.
        // So, we use the window ID as a means of getting and destroying the correct window.
        m_Application.DestroyWindowByID(WindowCloseEvent.GetSourceWindowID());
        
        return true; 
    });
}

FWindowSpecification CFunkinLoop::BuildWindowSpecification() const
{
    FWindowSpecification WindowSpecification = {};
    
    if (WindowSpecification.WindowMode == EWindowMode::Windowed || WindowSpecification.WindowMode == EWindowMode::Fullscreen)
    {
        int32 CmdLineWindowWidth = 0;
        int32 CmdLineWindowHeight = 0;

        if (CCommandLine::Get().Value("ResolutionX", &CmdLineWindowWidth))
        {
            if (CmdLineWindowWidth != 0)
                WindowSpecification.Width = CmdLineWindowWidth;
        }

        if (CCommandLine::Get().Value("ResolutionY", &CmdLineWindowHeight))
        {
            if (CmdLineWindowHeight != 0)
                WindowSpecification.Height = CmdLineWindowHeight;
        }
    }
    
    if (CCommandLine::Get().Flag("windowed"))
        WindowSpecification.WindowMode = EWindowMode::Windowed;
    
    if (CCommandLine::Get().Flag("windowedFullscreen"))
        WindowSpecification.WindowMode = EWindowMode::WindowedFullscreen;
    
    if (CCommandLine::Get().Flag("fullscreen"))
        WindowSpecification.WindowMode = EWindowMode::Fullscreen;
    
    if (CCommandLine::Get().Flag("vsync"))
        WindowSpecification.bEnableVSync = true;
    
    return WindowSpecification;
}

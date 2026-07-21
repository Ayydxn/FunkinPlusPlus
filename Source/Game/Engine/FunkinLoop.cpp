#include "FunkinPCH.h"
#include "FunkinLoop.h"
#include "Debug/Profiler.h"
#include "Events/WindowEvents.h"
#include "Logging/Logging.h"
#include "Misc/CommandLine.h"
#include "Misc/Paths.h"

bool CFunkinLoop::PreInitialize()
{
    CLogging::Initialize();
    
    FEngineDelegates::PreInitializeDelegate.Broadcast();
    
    return true;
}

bool CFunkinLoop::Initialize()
{
    LOG_INFO_TAG("Core", "Starting Friday Night Funkin++...");
    
    if (!m_Application.Initialize(m_EngineContext, BuildWindowSpecification()))
        return false;
    
    CPaths::Initialize();
    
    const uint32 MainWindowID = m_Application.GetMainWindow().GetNativeWindowID();
    const auto MainWindowWidth = m_Application.GetMainWindow().GetWidth();
    const auto MainWindowHeight = m_Application.GetMainWindow().GetHeight();
    const bool bWasVSyncRequested = m_Application.GetMainWindow().WantsVSync();
    
    if (!m_EngineContext.Initialize(ResolveRHIBackend(), MainWindowID, m_Application.GetMainWindow().GetNativeHandle(), MainWindowWidth, MainWindowHeight, bWasVSyncRequested))
        return false;
    
    m_ListenerHandle = m_EngineContext.GetEventBroadcaster().AddListener([this](IEvent& Event) { OnEvent(Event); }, 0);
    
    m_FramePacer.Initialize(1.0 / TickRate, ResolveDefaultRenderIntervalSeconds());
    
    m_TrianglePipeline = m_EngineContext.GetRenderer().CreateGraphicsPipeline(*m_EngineContext.GetRenderer().GetShader("TriangleTest"));
    
    FEngineDelegates::InitializeDelegate.Broadcast();
    
    bIsRunning = true;
    
    return true;
}

void CFunkinLoop::Tick()
{
    while (bIsRunning)
    {
        m_FramePacer.WaitForNextDeadline();
        
        if (m_FramePacer.IsTickDue())
        {
            FUNKIN_PROFILE_SCOPE("Tick")
            
            m_EngineContext.GetInputState().BeginFrame();
        
            m_Application.PumpMessages();
        
            m_EngineContext.GetInputState().EndFrame();
            
            // TODO: (Ayydxn) Stuff related to gameplay goes here.
            
            m_FramePacer.OnTickExecuted();
        }
        
        if (m_FramePacer.IsRenderDue())
        {
            FUNKIN_PROFILE_SCOPE("Render")
            
            if (m_EngineContext.GetRenderer().BeginFrame(m_Application.GetMainWindow().GetNativeWindowID()))
            {
                /* -- TEMPORARY: Triangle rendering test -- */
                
                m_EngineContext.GetRenderer().BindPipeline(m_Application.GetMainWindow().GetNativeWindowID(), *m_TrianglePipeline);
                m_EngineContext.GetRenderer().Draw(m_Application.GetMainWindow().GetNativeWindowID(), 3, 1);
                
                /* -- TEMPORARY: Triangle rendering test -- */
                
                m_EngineContext.GetRenderer().EndFrame(m_Application.GetMainWindow().GetNativeWindowID());
            }
            
            m_FramePacer.OnRenderExecuted();
            
            FUNKIN_PROFILE_MARK_FRAME;
        }
        
        m_EngineContext.SetFrameStats(m_FramePacer.GetFrameStats());
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
    Dispatcher.Dispatch<CWindowResizeEvent>([this](const CWindowResizeEvent& WindowResizeEvent)
    {
        m_EngineContext.GetRHIContext().OnWindowResized(WindowResizeEvent.GetSourceWindowID(), WindowResizeEvent.GetWidth(), WindowResizeEvent.GetHeight());
        
        return true;
    });
    
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

double CFunkinLoop::ResolveDefaultRenderIntervalSeconds() const
{
    constexpr double FallbackFramerate = 240.0;

    // TODO: (Ayydxn) Once a config file for game settings exists, prefer a user-set max FPS from it over the display refresh rate query below.
    const float RefreshRate = m_Application.GetMainWindow().GetDisplayRefreshRate();
    if (RefreshRate > 0.0f)
        return 1.0 / static_cast<double>(RefreshRate);

    LOG_WARN_TAG("Core", "Failed to query display refresh rate, falling back to {} FPS cap", FallbackFramerate);
    
    return 1.0 / FallbackFramerate;
}

ERHIBackend CFunkinLoop::ResolveRHIBackend() const
{
    // TODO: (Ayydxn) Once we have game settings, read the RHI backend from there. Since we only support Vulkan right now, we just always return that.
    constexpr ERHIBackend SelectedRHIBackend = ERHIBackend::Vulkan;
    
    LOG_INFO_TAG("Renderer", "Selected RHI Backend: {}", GetRHIBackendName(SelectedRHIBackend));

    return SelectedRHIBackend;
}

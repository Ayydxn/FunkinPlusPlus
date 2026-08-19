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
    
    const auto MainWindowWidth = m_Application.GetWindow().GetWidth();
    const auto MainWindowHeight = m_Application.GetWindow().GetHeight();
    const bool bWasVSyncRequested = m_Application.GetWindow().WantsVSync();
    
    if (!m_EngineContext.Initialize(ResolveRHIBackend(), m_Application.GetWindow().GetNativeHandle(), MainWindowWidth, MainWindowHeight, bWasVSyncRequested))
        return false;
    
    m_ListenerHandle = m_EngineContext.GetEventBroadcaster().AddListener([this](IEvent& Event) { OnEvent(Event); }, 0);
    
    m_FramePacer.Initialize(1.0 / TickRate, ResolveDefaultRenderIntervalSeconds());

    FEngineDelegates::InitializeDelegate.Broadcast();
    
    /* -- TEMPORARY: Triangle rendering test -- */
    
    // Vertices coordinates
    const float Vertices[] =
    {
        -0.5f, -0.5f * static_cast<float>(sqrt(3)) / 3, 0.0f, // Lower left corner
        0.5f, -0.5f * static_cast<float>(sqrt(3)) / 3, 0.0f, // Lower right corner
        0.0f, 0.5f * static_cast<float>(sqrt(3)) * 2 / 3, 0.0f, // Upper corner
        -0.5f / 2, 0.5f * static_cast<float>(sqrt(3)) / 6, 0.0f, // Inner left
        0.5f / 2, 0.5f * static_cast<float>(sqrt(3)) / 6, 0.0f, // Inner right
        0.0f, -0.5f * static_cast<float>(sqrt(3)) / 3, 0.0f // Inner down
    };

    // Indices for vertices order
    const uint32 Indices[] =
    {
        0, 3, 5, // Lower left triangle
        3, 2, 4, // Upper triangle
        5, 4, 1 // Lower right triangle
    };
    
    FVertexBufferDescription VertexBufferDescription;
    VertexBufferDescription.InitialData = Vertices;
    VertexBufferDescription.SizeInBytes = sizeof(Vertices);
    VertexBufferDescription.Layout = {
        { "Positions", EShaderDataType::Float3 }
    };
    
    FIndexBufferDescription IndexBufferDescription;
    IndexBufferDescription.InitialData = Indices;
    IndexBufferDescription.SizeInBytes = sizeof(Indices);
    
    m_VertexBuffer = CreateVertexBuffer(ResolveRHIBackend(), m_EngineContext.GetRHIContext(), VertexBufferDescription);
    m_IndexBuffer = CreateIndexBuffer(ResolveRHIBackend(), m_EngineContext.GetRHIContext(), IndexBufferDescription);
    
    /* -- TEMPORARY: Triangle rendering test -- */
    
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
            
            if (!bIsWindowMinimized && m_EngineContext.GetRenderer().BeginFrame())
            {
                /* -- TEMPORARY: Triangle rendering test -- */
                
                m_EngineContext.GetRenderer().BindPipeline(*m_EngineContext.GetRenderer().GetGraphicsPipelineManager().GetGraphicsPipeline("TriangleTest"));
                m_EngineContext.GetRenderer().BindVertexBuffer(*m_VertexBuffer);
                m_EngineContext.GetRenderer().BindIndexBuffer(*m_IndexBuffer);
                
                // (Ayydxn) Currently, you don't see anything because Vulkan's NDC making Y+ down and some shenanigans related to that.
                // I'll just have to come back and deal with that when cameras become a thing.
                m_EngineContext.GetRenderer().DrawIndexed(m_IndexBuffer->GetCount(), 1);
                
                /* -- TEMPORARY: Triangle rendering test -- */
                
                /* -- Render ImGui -- */
                
                m_EngineContext.GetImGuiRenderer().BeginFrame();
                
                m_EngineContext.GetImGuiRenderer().EndFrame(m_EngineContext.GetRenderer().GetCurrentCommandBuffer());
                
                /*--------------------*/
                
                m_EngineContext.GetRenderer().EndFrame();
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
    
    /* -- TEMPORARY: Triangle rendering test -- */
    
    m_VertexBuffer.reset();
    m_IndexBuffer.reset();
    
    /* -- TEMPORARY: Triangle rendering test -- */
    
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
        m_EngineContext.GetRHIContext().OnWindowResized(WindowResizeEvent.GetWidth(), WindowResizeEvent.GetHeight());
        
        return true;
    });
    
    Dispatcher.Dispatch<CWindowMinimizeEvent>([this](const CWindowMinimizeEvent& WindowMinimizeEvent)
    {
        bIsWindowMinimized = WindowMinimizeEvent.IsWindowMinimized();
        
        if (bIsWindowMinimized)
            m_EngineContext.GetRHIContext().OnWindowResized(0, 0);
        
        return true;
    });
    
    Dispatcher.Dispatch<CWindowCloseEvent>([this](const CWindowCloseEvent& WindowCloseEvent)
    {
        bIsRunning = false;
        
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
    const float RefreshRate = m_Application.GetWindow().GetDisplayRefreshRate();
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

#include "FunkinPCH.h"
#include "Application.h"
#include "Debug/Profiler.h"

#include <SDL3/SDL.h>

bool CApplication::Initialize(CEngineContext& EngineContext, const FWindowSpecification& WindowSpecification)
{
    m_EngineContext = &EngineContext;

    const int32 SDLVersion = SDL_GetVersion();
    LOG_INFO_TAG("Core", "SDL Version: {}.{}.{}", SDL_VERSIONNUM_MAJOR(SDLVersion), SDL_VERSIONNUM_MINOR(SDLVersion), SDL_VERSIONNUM_MICRO(SDLVersion));
    
    verifyFunkinf(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD), "Failed to initialize SDL3: {}", SDL_GetError())
    
    m_MainWindow = std::make_unique<CWindow>();
    if (!m_MainWindow->Initialize(WindowSpecification))
        return false;
    
    m_MainWindow->SetEventCallbackFunction([this](IEvent& Event)
    {
        m_EngineContext->GetEventBroadcaster().Broadcast(Event);
    });
    
    return true;
}

void CApplication::Shutdown()
{
    m_SecondaryWindows.clear();
    m_MainWindow.reset();
    
    SDL_Quit();
}

void CApplication::PumpMessages()
{
    FUNKIN_PROFILE_FUNCTION()
    
    SDL_Event Event;
    while (SDL_PollEvent(&Event))
    {
        const uint64 CaptureTimestampNs = SDL_GetTicksNS();
        const uint32 WindowID = Event.window.windowID;
        
        if (WindowID == m_MainWindow->GetNativeWindowID())
        {
            m_MainWindow->HandleNativeEvent(Event, CaptureTimestampNs);
            continue;
        }
        
        if (const auto Iterator = m_SecondaryWindows.find(WindowID); Iterator != m_SecondaryWindows.end())
            Iterator->second->HandleNativeEvent(Event, CaptureTimestampNs);
    }
}

CWindow* CApplication::MakeWindow(const FWindowSpecification& Specification)
{
    auto Window = std::make_unique<CWindow>();
    if (!Window->Initialize(Specification))
        return nullptr;

    Window->SetEventCallbackFunction([this](IEvent& Event)
    {
        m_EngineContext->GetEventBroadcaster().Broadcast(Event);
    });

    CWindow* RawWindow = Window.get();
    m_SecondaryWindows[Window->GetNativeWindowID()] = std::move(Window);

    return RawWindow;
}

void CApplication::DestroyWindow(const CWindow* Window)
{
    m_SecondaryWindows.erase(Window->GetNativeWindowID());
}

void CApplication::DestroyWindowByID(uint32 WindowID)
{
    m_SecondaryWindows.erase(WindowID);
}

#include "FunkinPCH.h"
#include "Application.h"
#include "Debug/Profiler.h"
#include "Events/GamepadEvents.h"

#include <SDL3/SDL.h>

namespace
{
    std::optional<EGamepadButton> SDLGamepadButtonToFunkinButton(const SDL_GamepadButton SDLGamepadButton)
    {
        switch (SDLGamepadButton)
        {
            case SDL_GAMEPAD_BUTTON_SOUTH:           return EGamepadButton::A;
            case SDL_GAMEPAD_BUTTON_EAST:            return EGamepadButton::B;
            case SDL_GAMEPAD_BUTTON_WEST:            return EGamepadButton::X;
            case SDL_GAMEPAD_BUTTON_NORTH:           return EGamepadButton::Y;
            case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:   return EGamepadButton::LeftBumper;
            case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:  return EGamepadButton::RightBumper;
            case SDL_GAMEPAD_BUTTON_BACK:            return EGamepadButton::Back;
            case SDL_GAMEPAD_BUTTON_START:           return EGamepadButton::Start;
            case SDL_GAMEPAD_BUTTON_GUIDE:           return EGamepadButton::Guide;
            case SDL_GAMEPAD_BUTTON_LEFT_STICK:      return EGamepadButton::LeftThumbstick;
            case SDL_GAMEPAD_BUTTON_RIGHT_STICK:     return EGamepadButton::RightThumbstick;
            case SDL_GAMEPAD_BUTTON_DPAD_UP:         return EGamepadButton::DPadUp;
            case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:      return EGamepadButton::DPadRight;
            case SDL_GAMEPAD_BUTTON_DPAD_DOWN:       return EGamepadButton::DPadDown;
            case SDL_GAMEPAD_BUTTON_DPAD_LEFT:       return EGamepadButton::DPadLeft;
            default: return std::nullopt;
        }
    }
}

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
        
        if (HandleGamepadEvent(Event, CaptureTimestampNs))
            continue;
        
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
    
    if (!m_EngineContext->RegisterWindow(Window->GetNativeWindowID(), Window->GetNativeHandle(), Window->GetWidth(), Window->GetHeight(), Window->WantsVSync()))
        return nullptr;

    CWindow* RawWindow = Window.get();
    m_SecondaryWindows[Window->GetNativeWindowID()] = std::move(Window);

    return RawWindow;
}

void CApplication::DestroyWindow(const CWindow* Window)
{
    m_EngineContext->UnregisterWindow(Window->GetNativeWindowID());
    
    m_SecondaryWindows.erase(Window->GetNativeWindowID());
}

void CApplication::DestroyWindowByID(uint32 WindowID)
{
    m_EngineContext->UnregisterWindow(WindowID);
    
    m_SecondaryWindows.erase(WindowID);
}

bool CApplication::HandleGamepadEvent(const SDL_Event& Event, uint64 CaptureTimestampNs) const
{
    if (Event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN)
    {
        if (const std::optional<EGamepadButton> GamepadButton = SDLGamepadButtonToFunkinButton(static_cast<SDL_GamepadButton>(Event.gbutton.button)))
        {
            CGamepadButtonPressedEvent GamepadButtonPressedEvent(Event.gbutton.which, *GamepadButton, CaptureTimestampNs);
            m_EngineContext->GetEventBroadcaster().Broadcast(GamepadButtonPressedEvent);
        }

        return true;
    }

    if (Event.type == SDL_EVENT_GAMEPAD_BUTTON_UP)
    {
        if (const std::optional<EGamepadButton> GamepadButton = SDLGamepadButtonToFunkinButton(static_cast<SDL_GamepadButton>(Event.gbutton.button)))
        {
            CGamepadButtonReleasedEvent GamepadButtonReleasedEvent(Event.gbutton.which, *GamepadButton, CaptureTimestampNs);
            m_EngineContext->GetEventBroadcaster().Broadcast(GamepadButtonReleasedEvent);
        }

        return true;
    }
    
    if (Event.type == SDL_EVENT_GAMEPAD_ADDED)
    {
        CGamepadConnectedEvent GamepadConnectedEvent(Event.gdevice.which);
        GamepadConnectedEvent.TimestampNs = CaptureTimestampNs;
            
        m_EngineContext->GetEventBroadcaster().Broadcast(GamepadConnectedEvent);
        
        return true;
    }

    if (Event.type == SDL_EVENT_GAMEPAD_REMOVED)
    {
        CGamepadDisconnectedEvent GamepadDisconnectedEvent(Event.gdevice.which);
        GamepadDisconnectedEvent.TimestampNs = CaptureTimestampNs;
            
        m_EngineContext->GetEventBroadcaster().Broadcast(GamepadDisconnectedEvent);
        
        return true;
    }
    
    return false;
}

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
    
    m_Window = std::make_unique<CWindow>();
    if (!m_Window->Initialize(WindowSpecification))
        return false;
    
    m_Window->SetEventCallbackFunction([this](IEvent& Event)
    {
        m_EngineContext->GetEventBroadcaster().Broadcast(Event);
    });
    
    return true;
}

void CApplication::Shutdown()
{
    m_Window.reset();
    
    SDL_Quit();
}

void CApplication::PumpMessages()
{
    FUNKIN_PROFILE_FUNCTION()
    
    SDL_Event Event;
    while (SDL_PollEvent(&Event))
    {
        const uint64 CaptureTimestampNs = SDL_GetTicksNS();
        
        if (HandleGamepadEvent(Event, CaptureTimestampNs))
            continue;
        
        m_Window->HandleNativeEvent(Event, CaptureTimestampNs);
    }
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

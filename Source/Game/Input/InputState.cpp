#include "FunkinPCH.h"
#include "InputState.h"
#include "Debug/Profiler.h"
#include "Events/GamepadEvents.h"
#include "Events/KeyboardEvents.h"
#include "Events/MouseEvents.h"
#include "Logging/Logging.h"

#include <SDL3/SDL.h>

#include <ranges>

void CInputState::Initialize(CEventBroadcaster& EventBroadcaster)
{
    FKeys::InitializeKeys();

    // Priority 0 puts this ahead of anything that wants to react to *processed* input state
    // rather than the raw event (gameplay action bindings, UI focus handling, etc.) -- see
    // CEventBroadcaster::AddListener's own doc comment on priority ordering.
    // This listener never marks events as handled, so it never blocks propagation to anything downstream.
    m_ListenerHandle = EventBroadcaster.AddListener([this](IEvent& Event) { OnEvent(Event); }, 0);
}

void CInputState::Shutdown(CEventBroadcaster& EventBroadcaster) const
{
    EventBroadcaster.RemoveListener(m_ListenerHandle);

    for (const CGamepadState& GamepadState : m_GamepadStates)
    {
        if (GamepadState.bConnected && GamepadState.Handle)
            SDL_CloseGamepad(GamepadState.Handle);
    }
}

void CInputState::BeginFrame()
{
    FUNKIN_PROFILE_FUNCTION()

    m_MouseState.PreviousX = m_MouseState.CurrentX;
    m_MouseState.PreviousY = m_MouseState.CurrentY;
    m_MouseState.ScrollX = 0.0f;
    m_MouseState.ScrollY = 0.0f;

    TransitionPressedKeys();
    TransitionPressedMouseButtons();
    PollGamepads();
}

void CInputState::EndFrame()
{
    FUNKIN_PROFILE_FUNCTION()

    m_MouseState.DeltaX = 0.0f;
    m_MouseState.DeltaY = 0.0f;

    ClearReleasedKeys();
}

/*----------------*/
/* -- Keyboard -- */
/*----------------*/

bool CInputState::IsKeyDown(const FKey& Key) const
{
    const auto It = m_KeyData.find(Key.GetKeyCode());

    return It != m_KeyData.end() && It->second.CurrentKeyState == EKeyState::Pressed;
}

bool CInputState::IsKeyHeld(const FKey& Key) const
{
    const auto It = m_KeyData.find(Key.GetKeyCode());

    return It != m_KeyData.end() && (It->second.CurrentKeyState == EKeyState::Pressed || It->second.CurrentKeyState == EKeyState::Held);
}

bool CInputState::IsKeyUp(const FKey& Key) const
{
    const auto It = m_KeyData.find(Key.GetKeyCode());

    return It != m_KeyData.end() && It->second.CurrentKeyState == EKeyState::Released;
}

/*-------------*/
/* -- Mouse -- */
/*-------------*/

bool CInputState::IsMouseButtonDown(const FKey& MouseButton) const
{
    const auto It = m_MouseButtonData.find(MouseButton.GetKeyCode());

    return It != m_MouseButtonData.end() && It->second.CurrentState == EKeyState::Pressed;
}

bool CInputState::IsMouseButtonHeld(const FKey& MouseButton) const
{
    const auto It = m_MouseButtonData.find(MouseButton.GetKeyCode());

    return It != m_MouseButtonData.end() && (It->second.CurrentState == EKeyState::Pressed || It->second.CurrentState == EKeyState::Held);
}

bool CInputState::IsMouseButtonUp(const FKey& MouseButton) const
{
    const auto It = m_MouseButtonData.find(MouseButton.GetKeyCode());

    return It != m_MouseButtonData.end() && It->second.CurrentState == EKeyState::Released;
}

/*----------------*/
/* -- Gamepads -- */
/*----------------*/

std::string_view CInputState::GetGamepadName(int32 GamepadIndex) const
{
    if (!IsValidGamepadIndex(GamepadIndex))
        return "Invalid Gamepad";

    return m_GamepadStates[GamepadIndex].Name;
}

bool CInputState::IsGamepadButtonDown(int32 GamepadIndex, EGamepadButton Button) const
{
    if (!IsGamepadConnected(GamepadIndex))
        return false;

    return m_GamepadStates[GamepadIndex].GetButtonState(Button) == EKeyState::Pressed;
}

bool CInputState::IsGamepadButtonHeld(int32 GamepadIndex, EGamepadButton Button) const
{
    if (!IsGamepadConnected(GamepadIndex))
        return false;

    const EKeyState State = m_GamepadStates[GamepadIndex].GetButtonState(Button);
    return State == EKeyState::Pressed || State == EKeyState::Held;
}

bool CInputState::IsGamepadButtonUp(int32 GamepadIndex, EGamepadButton Button) const
{
    if (!IsGamepadConnected(GamepadIndex))
        return false;

    return m_GamepadStates[GamepadIndex].GetButtonState(Button) == EKeyState::Released;
}

float CInputState::GetGamepadAxis(int32 GamepadIndex, EGamepadAxis Axis, float DeadZone) const
{
    if (!IsGamepadConnected(GamepadIndex))
        return 0.0f;

    return ApplyDeadZone(m_GamepadStates[GamepadIndex].GetRawAxis(Axis), DeadZone);
}

/*-----------------------*/
/* -- Action Mappings -- */
/*-----------------------*/

bool CInputState::GetAction(const std::string& ActionName) const
{
    const auto Iterator = m_ActionBindings.find(ActionName);
    if (Iterator == m_ActionBindings.end())
        return false;

    for (const auto& [SourceType, Trigger, Key, GamepadIndex, GamepadButton] : Iterator->second)
    {
        bool bTriggered = false;

        if (SourceType == FInputActionBinding::EActionSourceType::Key)
        {
            switch (Trigger)
            {
                case EInputActionTrigger::Pressed:  bTriggered = IsKeyDown(Key); break;
                case EInputActionTrigger::Released: bTriggered = IsKeyUp(Key); break;
                case EInputActionTrigger::Held:     bTriggered = IsKeyHeld(Key); break;
            }
        }
        else
        {
            switch (Trigger)
            {
                case EInputActionTrigger::Pressed:  bTriggered = IsGamepadButtonDown(GamepadIndex, GamepadButton); break;
                case EInputActionTrigger::Released: bTriggered = IsGamepadButtonUp(GamepadIndex, GamepadButton); break;
                case EInputActionTrigger::Held:     bTriggered = IsGamepadButtonHeld(GamepadIndex, GamepadButton); break;
            }
        }

        if (bTriggered)
            return true;
    }

    return false;
}

/*---------------------*/
/* -- Axis Mappings -- */
/*---------------------*/

float CInputState::GetAxis(const std::string& AxisName) const
{
    const auto It = m_AxisBindings.find(AxisName);
    if (It == m_AxisBindings.end())
        return 0.0f;

    float Value = 0.0f;

    for (const auto& [SourceType, Key, Scale, GamepadIndex, GamepadAxis, AxisScale, DeadZone] : It->second)
    {
        if (SourceType == FInputAxisBinding::EActionSourceType::Key)
        {
            if (IsKeyHeld(Key))
                Value += Scale;
        }
        else
        {
            Value += GetGamepadAxis(GamepadIndex, GamepadAxis, DeadZone) * AxisScale;
        }
    }

    return Value;
}

/*--------------------------*/
/* -- Internal Functions -- */
/*--------------------------*/

void CInputState::OnEvent(IEvent& Event)
{
    CEventDispatcher Dispatcher(Event);
    Dispatcher.Dispatch<CKeyPressedEvent>([this](const CKeyPressedEvent& KeyPressedEvent)
    {
        UpdateKeyState(KeyPressedEvent.GetKey(), EKeyState::Pressed);
        
        return false;
    });
    
    Dispatcher.Dispatch<CKeyReleasedEvent>([this](const CKeyReleasedEvent& KeyReleasedEvent)
    {
        UpdateKeyState(KeyReleasedEvent.GetKey(), EKeyState::Released);
        
        return false;
    });
    
    Dispatcher.Dispatch<CMouseButtonPressedEvent>([this](const CMouseButtonPressedEvent& MouseButtonPressedEvent)
    {
        UpdateMouseButtonState(MouseButtonPressedEvent.GetMouseButton(), EKeyState::Pressed);
        
        return false;
    });
    
    Dispatcher.Dispatch<CMouseButtonReleasedEvent>([this](const CMouseButtonReleasedEvent& MouseButtonReleasedEvent)
    {
        UpdateMouseButtonState(MouseButtonReleasedEvent.GetMouseButton(), EKeyState::Released);
        
        return false;
    });
    
    Dispatcher.Dispatch<CMouseMovedEvent>([this](const CMouseMovedEvent& MouseMovedEvent)
    {
        m_MouseState.DeltaX = MouseMovedEvent.GetXPosition() - m_MouseState.CurrentX;
        m_MouseState.DeltaY = MouseMovedEvent.GetYPosition() - m_MouseState.CurrentY;
        m_MouseState.CurrentX = MouseMovedEvent.GetXPosition();
        m_MouseState.CurrentY = MouseMovedEvent.GetYPosition();
        
        return false;
    });
    
    Dispatcher.Dispatch<CMouseScrolledEvent>([this](const CMouseScrolledEvent& MouseScrolledEvent)
    {
        m_MouseState.ScrollX += MouseScrolledEvent.GetXOffset();
        m_MouseState.ScrollY += MouseScrolledEvent.GetYOffset();
        
        return false;
    });
    
    Dispatcher.Dispatch<CGamepadConnectedEvent>([this](const CGamepadConnectedEvent& GamepadConnectedEvent)
    {
        OnGamepadConnected(GamepadConnectedEvent.GetGamepadID());
        
        return false;
    });
    
    Dispatcher.Dispatch<CGamepadDisconnectedEvent>([this](const CGamepadDisconnectedEvent& GamepadDisconnectedEvent)
    {
        OnGamepadDisconnected(GamepadDisconnectedEvent.GetGamepadID());
        
        return false;
    });
}

void CInputState::UpdateKeyState(const FKey& KeyToUpdate, EKeyState NewState)
{
    if (!KeyToUpdate.IsValid())
        return;

    auto& [Key, CurrentKeyState, OldKeyState] = m_KeyData[KeyToUpdate.GetKeyCode()];
    Key = KeyToUpdate;
    OldKeyState = CurrentKeyState;
    CurrentKeyState = NewState;
}

void CInputState::UpdateMouseButtonState(const FKey& MouseButtonToUpdate, EKeyState NewState)
{
    if (!MouseButtonToUpdate.IsValid())
        return;

    auto& [MouseButton, CurrentState, OldState] = m_MouseButtonData[MouseButtonToUpdate.GetKeyCode()];
    MouseButton = MouseButtonToUpdate;
    OldState = CurrentState;
    CurrentState = NewState;
}

void CInputState::OnGamepadConnected(uint32 GamepadID)
{
    for (int32 i = 0; i < GMaxGamepadCount; ++i)
    {
        if (!m_GamepadStates[i].bConnected)
        {
            SDL_Gamepad* GamepadHandle = SDL_OpenGamepad(GamepadID);
            if (!GamepadHandle)
            {
                LOG_WARN_TAG("Input", "Failed to open gamepad {}: {}", GamepadID, SDL_GetError());
                return;
            }

            m_GamepadStates[i].Handle = GamepadHandle;
            m_GamepadStates[i].bConnected = true;
            m_GamepadStates[i].Name = SDL_GetGamepadName(GamepadHandle) ? SDL_GetGamepadName(GamepadHandle) : "Unknown";

            LOG_INFO_TAG("Input", "Gamepad {} ('{}') connected.", i, m_GamepadStates[i].Name);
            return;
        }
    }

    LOG_WARN_TAG("Input", "Maximum gamepad count ({}) reached; ignoring new gamepad.", GMaxGamepadCount);
}

void CInputState::OnGamepadDisconnected(uint32 GamepadID)
{
    for (int32 i = 0; i < GMaxGamepadCount; ++i)
    {
        CGamepadState& State = m_GamepadStates[i];
        if (State.bConnected && State.Handle && SDL_GetJoystickID(SDL_GetGamepadJoystick(State.Handle)) == GamepadID)
        {
            LOG_INFO_TAG("Input", "Gamepad {} ('{}') disconnected.", i, State.Name);

            SDL_CloseGamepad(State.Handle);
            State = CGamepadState {};

            return;
        }
    }
}

void CInputState::TransitionPressedKeys()
{
    for (auto& Data : m_KeyData | std::views::values)
    {
        if (Data.CurrentKeyState == EKeyState::Pressed)
            UpdateKeyState(Data.Key, EKeyState::Held);
    }
}

void CInputState::TransitionPressedMouseButtons()
{
    for (auto& Data : m_MouseButtonData | std::views::values)
    {
        if (Data.CurrentState == EKeyState::Pressed)
            UpdateMouseButtonState(Data.MouseButton, EKeyState::Held);
    }
}

void CInputState::ClearReleasedKeys()
{
    for (auto& Data : m_KeyData | std::views::values)
    {
        if (Data.CurrentKeyState == EKeyState::Released)
            UpdateKeyState(Data.Key, EKeyState::Unknown);
    }

    for (auto& Data : m_MouseButtonData | std::views::values)
    {
        if (Data.CurrentState == EKeyState::Released)
            UpdateMouseButtonState(Data.MouseButton, EKeyState::Unknown);
    }
}

void CInputState::PollGamepads()
{
    FUNKIN_PROFILE_FUNCTION()

    for (CGamepadState& GamepadState : m_GamepadStates)
    {
        if (!GamepadState.bConnected || !GamepadState.Handle)
            continue;

        for (uint8 ButtonIndex = 0; ButtonIndex < CGamepadState::ButtonCount; ++ButtonIndex)
        {
            const bool bIsButtonDown = SDL_GetGamepadButton(GamepadState.Handle, static_cast<SDL_GamepadButton>(ButtonIndex));
            EKeyState& ButtonState = GamepadState.ButtonStates[ButtonIndex];

            if (bIsButtonDown)
            {
                ButtonState = (ButtonState == EKeyState::Pressed || ButtonState == EKeyState::Held) ? EKeyState::Held : EKeyState::Pressed;
            }
            else
            {
                ButtonState = (ButtonState == EKeyState::Released || ButtonState == EKeyState::Unknown) ? EKeyState::Unknown : EKeyState::Released;
            }
        }

        for (uint8 AxisIndex = 0; AxisIndex < CGamepadState::AxisCount; ++AxisIndex)
        {
            const int16 RawValue = SDL_GetGamepadAxis(GamepadState.Handle, static_cast<SDL_GamepadAxis>(AxisIndex));

            const bool bIsTrigger = (AxisIndex == static_cast<uint8>(EGamepadAxis::LeftTrigger) || AxisIndex == static_cast<uint8>(EGamepadAxis::RightTrigger));
            
            GamepadState.RawAxisValues[AxisIndex] = bIsTrigger
                ? static_cast<float>(RawValue) / 32767.0f
                : static_cast<float>(RawValue) / (RawValue < 0 ? 32768.0f : 32767.0f);
        }
    }
}

float CInputState::ApplyDeadZone(float Value, float DeadZone)
{
    if (std::abs(Value) < DeadZone)
        return 0.0f;

    const float Sign = Value < 0.0f ? -1.0f : 1.0f;
    return Sign * ((std::abs(Value) - DeadZone) / (1.0f - DeadZone));
}

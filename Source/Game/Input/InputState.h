#pragma once

#include "CoreInputTypes.h"
#include "GamepadTypes.h"
#include "InputBindings.h"
#include "TimestampedInputEvent.h"
#include "Core/Events/EventBroadcaster.h"

#include <string>
#include <unordered_map>
#include <vector>

class CWindow;

class CInputState
{
public:
    void Initialize(CEventBroadcaster& EventBroadcaster);
    void Shutdown(CEventBroadcaster& EventBroadcaster) const;
    void BeginFrame();
    void EndFrame();

    /*----------------*/
    /* -- Keyboard -- */
    /*----------------*/

    bool IsKeyDown(const FKey& Key) const;
    bool IsKeyHeld(const FKey& Key) const;
    bool IsKeyUp(const FKey& Key) const;
    bool IsShiftHeld() const { return IsKeyHeld(FKeys::LeftShift) || IsKeyHeld(FKeys::RightShift); }
    bool IsControlHeld() const { return IsKeyHeld(FKeys::LeftControl) || IsKeyHeld(FKeys::RightControl); }
    bool IsAltHeld() const { return IsKeyHeld(FKeys::LeftAlt) || IsKeyHeld(FKeys::RightAlt); }

    /*-------------*/
    /* -- Mouse -- */
    /*-------------*/

    bool IsMouseButtonDown(const FKey& MouseButton) const;
    bool IsMouseButtonHeld(const FKey& MouseButton) const;
    bool IsMouseButtonUp(const FKey& MouseButton) const;

    float GetMouseX() const { return m_MouseState.CurrentX; }
    float GetMouseY() const { return m_MouseState.CurrentY; }
    std::pair<float, float> GetMousePosition() const { return { m_MouseState.CurrentX, m_MouseState.CurrentY }; }
    float GetMouseDeltaX() const { return m_MouseState.GetDeltaX(); }
    float GetMouseDeltaY() const { return m_MouseState.GetDeltaY(); }
    std::pair<float, float> GetMouseDelta() const { return { m_MouseState.GetDeltaX(), m_MouseState.GetDeltaY() }; }

    float GetScrollX() const { return m_MouseState.ScrollX; }
    float GetScrollY() const { return m_MouseState.ScrollY; }

    /*----------------*/
    /* -- Gamepads -- */
    /*----------------*/

    bool IsGamepadConnected(int32 GamepadIndex) const { return IsValidGamepadIndex(GamepadIndex) && m_GamepadStates[GamepadIndex].bConnected; }

    std::string_view GetGamepadName(int32 GamepadIndex) const;

    bool IsGamepadButtonDown(int32 GamepadIndex, EGamepadButton Button) const;
    bool IsGamepadButtonHeld(int32 GamepadIndex, EGamepadButton Button) const;
    bool IsGamepadButtonUp(int32 GamepadIndex, EGamepadButton Button) const;
    float GetGamepadAxis(int32 GamepadIndex, EGamepadAxis Axis, float DeadZone = 0.15f) const;

    /*-----------------------*/
    /* -- Action Mappings -- */
    /*-----------------------*/

    void RegisterAction(const std::string& ActionName, const FInputActionBinding& Binding) { m_ActionBindings[ActionName].push_back(Binding); }
    void UnregisterAction(const std::string& ActionName) { m_ActionBindings.erase(ActionName); }
    bool GetAction(const std::string& ActionName) const;

    /*---------------------*/
    /* -- Axis Mappings -- */
    /*---------------------*/

    void RegisterAxis(const std::string& AxisName, const FInputAxisBinding& Binding) { m_AxisBindings[AxisName].push_back(Binding); }
    void UnregisterAxis(const std::string& AxisName) { m_AxisBindings.erase(AxisName); }
    float GetAxis(const std::string& AxisName) const;
    
    /*----------------------------*/
    /* -- Timestamped Events  -- */
    /*----------------------------*/
    
    const std::vector<FTimestampedInputEvent>& GetTickEvents() const { return m_TickEvents; }
private:
    void OnEvent(IEvent& Event);

    void UpdateKeyState(const FKey& KeyToUpdate, EKeyState NewState);
    void UpdateMouseButtonState(const FKey& MouseButtonToUpdate, EKeyState NewState);

    void OnGamepadConnected(uint32 GamepadID);
    void OnGamepadDisconnected(uint32 GamepadID);
    void OnGamepadButtonChanged(uint32 GamepadID, EGamepadButton Button, bool bPressed, uint64 TimestampNs);
    int32 FindGamepadSlotByID(uint32 GamepadID) const;

    void TransitionPressedKeys();
    void TransitionPressedMouseButtons();
    void TransitionPressedGamepadButtons();
    
    void ClearReleasedKeys();
    void ClearReleasedGamepadButtons();

    void PollGamepads();
    static bool IsValidGamepadIndex(int32 Index) { return Index >= 0 && Index < GMaxGamepadCount; }
    static float ApplyDeadZone(float Value, float DeadZone);

private:
    CEventBroadcaster::FListenerHandle m_ListenerHandle = 0;

    std::unordered_map<uint32, FKeyData> m_KeyData;
    std::unordered_map<uint32, FMouseButtonData> m_MouseButtonData;
    FMouseState m_MouseState;

    std::array<CGamepadState, GMaxGamepadCount> m_GamepadStates = {};

    std::unordered_map<std::string, std::vector<FInputActionBinding>> m_ActionBindings;
    std::unordered_map<std::string, std::vector<FInputAxisBinding>> m_AxisBindings;
    
    std::vector<FTimestampedInputEvent> m_TickEvents;
};

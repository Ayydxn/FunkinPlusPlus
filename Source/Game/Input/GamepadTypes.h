#pragma once

#include "CoreInputTypes.h"
#include "Core/CoreTypes.h"

#include <array>
#include <string>

struct SDL_Gamepad;

static constexpr int32 GMaxGamepadCount = 4;

enum class EGamepadButton : uint8
{
    // Xbox/Generic Controllers
    A = 0,
    B = 1,
    X = 2,
    Y = 3,

    // PlayStation Controllers
    Cross = A,
    Circle = B,
    Square = X,
    Triangle = Y,

    LeftBumper = 4,
    RightBumper = 5,
    Back = 6,
    Start = 7,
    Guide = 8, // "Home" / Xbox button — may not be available on all controllers
    LeftThumbstick = 9, // Left stick click
    RightThumbstick = 10, // Right stick click
    DPadUp = 11,
    DPadRight = 12,
    DPadDown = 13,
    DPadLeft = 14
};

enum class EGamepadAxis : uint8
{
    LeftStickX = 0,
    LeftStickY = 1,
    RightStickX = 2,
    RightStickY = 3,
    LeftTrigger = 4,
    RightTrigger = 5
};

struct CGamepadState
{
    static constexpr int32 ButtonCount = 15;
    static constexpr int32 AxisCount = 6;

    SDL_Gamepad* Handle = nullptr;
    std::string Name;
    bool bConnected = false;
    std::array<EKeyState, ButtonCount> ButtonStates = {};
    std::array<float, AxisCount> RawAxisValues = {};

    EKeyState GetButtonState(EGamepadButton Button) const
    {
        return ButtonStates[static_cast<uint8>(Button)];
    }

    float GetRawAxis(EGamepadAxis Axis) const
    {
        return RawAxisValues[static_cast<uint8>(Axis)];
    }
};
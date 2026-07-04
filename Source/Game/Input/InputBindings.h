#pragma once

#include "CoreInputTypes.h"
#include "GamepadTypes.h"

enum class EInputActionTrigger : uint8
{
    Pressed,
    Released,
    Held
};

struct FInputActionBinding
{
    enum class EActionSourceType : uint8 { Key, GamepadButton };

    EActionSourceType SourceType = EActionSourceType::Key;
    EInputActionTrigger Trigger = EInputActionTrigger::Pressed;

    // Key source
    FKey Key;

    // Gamepad button source
    int32 GamepadIndex = 0;
    EGamepadButton GamepadButton = EGamepadButton::A;

    /*-----------------*/
    /* -- Factories -- */
    /*-----------------*/

    static FInputActionBinding FromKey(const FKey& Key, EInputActionTrigger Trigger = EInputActionTrigger::Pressed)
    {
        FInputActionBinding InputActionBinding;
        InputActionBinding.SourceType = EActionSourceType::Key;
        InputActionBinding.Key = Key;
        InputActionBinding.Trigger = Trigger;

        return InputActionBinding;
    }

    static FInputActionBinding FromGamepadButton(int32 GamepadIndex, EGamepadButton Button, EInputActionTrigger Trigger = EInputActionTrigger::Pressed)
    {
        FInputActionBinding InputActionBinding;
        InputActionBinding.SourceType = EActionSourceType::GamepadButton;
        InputActionBinding.GamepadIndex = GamepadIndex;
        InputActionBinding.GamepadButton = Button;
        InputActionBinding.Trigger = Trigger;

        return InputActionBinding;
    }
};

struct FInputAxisBinding
{
    enum class EActionSourceType : uint8 { Key, GamepadAxis };

    EActionSourceType SourceType = EActionSourceType::Key;

    // Key source — contributes Scale when held, 0 when not.
    FKey Key;
    float Scale = 1.0f;

    // Gamepad axis source — contributes RawAxis * AxisScale (after dead zone).
    int32 GamepadIndex = 0;
    EGamepadAxis GamepadAxis = EGamepadAxis::LeftStickX;
    float AxisScale = 1.0f;
    float DeadZone = 0.15f;

    /*-----------------*/
    /* -- Factories -- */
    /*-----------------*/

    static FInputAxisBinding FromKey(const FKey& Key, float Scale = 1.0f)
    {
        FInputAxisBinding InputAxisBinding;
        InputAxisBinding.SourceType = EActionSourceType::Key;
        InputAxisBinding.Key = Key;
        InputAxisBinding.Scale = Scale;

        return InputAxisBinding;
    }

    static FInputAxisBinding FromGamepadAxis(int32 GamepadIndex, EGamepadAxis Axis, float Scale = 1.0f, float DeadZone = 0.15f)
    {
        FInputAxisBinding InputAxisBinding;
        InputAxisBinding.SourceType = EActionSourceType::GamepadAxis;
        InputAxisBinding.GamepadIndex = GamepadIndex;
        InputAxisBinding.GamepadAxis = Axis;
        InputAxisBinding.AxisScale = Scale;
        InputAxisBinding.DeadZone = DeadZone;

        return InputAxisBinding;
    }
};
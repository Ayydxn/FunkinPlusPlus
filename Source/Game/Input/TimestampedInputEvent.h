#pragma once

#include "GamepadTypes.h"

struct FTimestampedInputEvent
{
    enum class ESourceType : uint8 { Key, GamepadButton };

    ESourceType SourceType = ESourceType::Key;
    bool bPressed = false; // true = pressed, false = released

    // Valid when SourceType == Key.
    FKey Key;

    // Valid when SourceType == GamepadButton. GamepadIndex refers to the engine's 0-3 slot index, not SDL's joystick instance ID.
    int32 GamepadIndex = 0;
    EGamepadButton GamepadButton = EGamepadButton::A;

    uint64 TimestampNs = 0;
};
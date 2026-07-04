#pragma once

#include "Core/CoreTypes.h"

#include <map>
#include <string>

enum class EKeyState : uint8
{
    Unknown,
    Pressed,
    Held,
    Released
};

struct FKey
{
    FKey() = default;

    FKey(std::string KeyName, uint32 KeyCode)
        : m_KeyName(std::move(KeyName)), m_KeyCode(KeyCode) {}

    const std::string& GetDisplayName() const { return m_KeyName; }
    uint32 GetKeyCode() const { return m_KeyCode; }

    bool IsValid() const { return m_KeyCode != InvalidKeyCode; }

    bool operator==(const FKey& Other) const { return m_KeyCode == Other.m_KeyCode; }
    bool operator!=(const FKey& Other) const { return m_KeyCode != Other.m_KeyCode; }

    static constexpr uint32 InvalidKeyCode = 0xFFFFFFFF;

    static FKey GetKeyFromKeyCode(uint32 KeyCode);
private:
    inline static std::map<uint32, FKey> m_KeyCodeToKeyMap;

    std::string m_KeyName = "Unknown Key";
    uint32 m_KeyCode = InvalidKeyCode;

    friend struct FKeys;
};

struct FKeyData
{
    FKey Key;
    EKeyState CurrentKeyState = EKeyState::Unknown;
    EKeyState OldKeyState = EKeyState::Unknown;
};

struct FMouseButtonData
{
    FKey MouseButton;
    EKeyState CurrentState = EKeyState::Unknown;
    EKeyState OldState = EKeyState::Unknown;
};

struct FMouseState
{
    float CurrentX = 0.0f;
    float CurrentY = 0.0f;
    float PreviousX = 0.0f;
    float PreviousY = 0.0f;
    float DeltaX = 0.0f;
    float DeltaY = 0.0f;

    float ScrollX = 0.0f;
    float ScrollY = 0.0f;

    float GetDeltaX() const { return DeltaX; }
    float GetDeltaY() const { return DeltaY; }
};

struct FKeys
{
    inline static const FKey Space = FKey("Space", 0x20);
    inline static const FKey Apostrophe = FKey("Apostrophe", 0x27);
    inline static const FKey Comma = FKey("Comma", 0x2C);
    inline static const FKey Minus = FKey("Minus", 0x2D);
    inline static const FKey Period = FKey("Period", 0x2E);
    inline static const FKey Slash = FKey("Slash", 0x2F);

    inline static const FKey Zero = FKey("Zero", 0x30);
    inline static const FKey One = FKey("One", 0x31);
    inline static const FKey Two = FKey("Two", 0x32);
    inline static const FKey Three = FKey("Three", 0x33);
    inline static const FKey Four = FKey("Four", 0x34);
    inline static const FKey Five = FKey("Five", 0x35);
    inline static const FKey Six = FKey("Six", 0x36);
    inline static const FKey Seven = FKey("Seven", 0x37);
    inline static const FKey Eight = FKey("Eight", 0x38);
    inline static const FKey Nine = FKey("Nine", 0x39);

    inline static const FKey Semicolon = FKey("Semicolon", 0x3B);
    inline static const FKey Equal = FKey("Equal", 0x3D);

    inline static const FKey A = FKey("A", 0x41);
    inline static const FKey B = FKey("B", 0x42);
    inline static const FKey C = FKey("C", 0x43);
    inline static const FKey D = FKey("D", 0x44);
    inline static const FKey E = FKey("E", 0x45);
    inline static const FKey F = FKey("F", 0x46);
    inline static const FKey G = FKey("G", 0x47);
    inline static const FKey H = FKey("H", 0x48);
    inline static const FKey I = FKey("I", 0x49);
    inline static const FKey J = FKey("J", 0x4A);
    inline static const FKey K = FKey("K", 0x4B);
    inline static const FKey L = FKey("L", 0x4C);
    inline static const FKey M = FKey("M", 0x4D);
    inline static const FKey N = FKey("N", 0x4E);
    inline static const FKey O = FKey("O", 0x4F);
    inline static const FKey P = FKey("P", 0x50);
    inline static const FKey Q = FKey("Q", 0x51);
    inline static const FKey R = FKey("R", 0x52);
    inline static const FKey S = FKey("S", 0x53);
    inline static const FKey T = FKey("T", 0x54);
    inline static const FKey U = FKey("U", 0x55);
    inline static const FKey V = FKey("V", 0x56);
    inline static const FKey W = FKey("W", 0x57);
    inline static const FKey X = FKey("X", 0x58);
    inline static const FKey Y = FKey("Y", 0x59);
    inline static const FKey Z = FKey("Z", 0x5A);

    inline static const FKey LeftBracket = FKey("Left Bracket", 0x5B);
    inline static const FKey Backslash = FKey("Backslash", 0x5C);
    inline static const FKey RightBracket = FKey("Right Bracket", 0x5D);
    inline static const FKey GraveAccent = FKey("Grave Accent", 0x60);

    /* -----------------------------------------------------------------------
       Keyboard — Function and navigation keys
    ----------------------------------------------------------------------- */

    inline static const FKey Escape = FKey("Escape", 0x100);
    inline static const FKey Enter = FKey("Enter", 0x101);
    inline static const FKey Tab = FKey("Tab", 0x102);
    inline static const FKey Backspace = FKey("Backspace", 0x103);
    inline static const FKey Insert = FKey("Insert", 0x104);
    inline static const FKey Delete = FKey("Delete", 0x105);
    inline static const FKey RightArrow = FKey("Right Arrow", 0x106);
    inline static const FKey LeftArrow = FKey("Left Arrow", 0x107);
    inline static const FKey DownArrow = FKey("Down Arrow", 0x108);
    inline static const FKey UpArrow = FKey("Up Arrow", 0x109);
    inline static const FKey PageUp = FKey("Page Up", 0x10A);
    inline static const FKey PageDown = FKey("Page Down", 0x10B);
    inline static const FKey Home = FKey("Home", 0x10C);
    inline static const FKey End = FKey("End", 0x10D);
    inline static const FKey CapsLock = FKey("Caps Lock", 0x118);
    inline static const FKey ScrollLock = FKey("Scroll Lock", 0x119);
    inline static const FKey NumberLock = FKey("Number Lock", 0x11A);
    inline static const FKey PrintScreen = FKey("Print Screen", 0x11B);
    inline static const FKey Pause = FKey("Pause", 0x11C);

    inline static const FKey F1 = FKey("F1", 0x122);
    inline static const FKey F2 = FKey("F2", 0x123);
    inline static const FKey F3 = FKey("F3", 0x124);
    inline static const FKey F4 = FKey("F4", 0x125);
    inline static const FKey F5 = FKey("F5", 0x126);
    inline static const FKey F6 = FKey("F6", 0x127);
    inline static const FKey F7 = FKey("F7", 0x128);
    inline static const FKey F8 = FKey("F8", 0x129);
    inline static const FKey F9 = FKey("F9", 0x12A);
    inline static const FKey F10 = FKey("F10", 0x12B);
    inline static const FKey F11 = FKey("F11", 0x12C);
    inline static const FKey F12 = FKey("F12", 0x12D);
    inline static const FKey F13 = FKey("F13", 0x12E);
    inline static const FKey F14 = FKey("F14", 0x12F);
    inline static const FKey F15 = FKey("F15", 0x130);
    inline static const FKey F16 = FKey("F16", 0x131);
    inline static const FKey F17 = FKey("F17", 0x132);
    inline static const FKey F18 = FKey("F18", 0x133);
    inline static const FKey F19 = FKey("F19", 0x134);
    inline static const FKey F20 = FKey("F20", 0x135);
    inline static const FKey F21 = FKey("F21", 0x136);
    inline static const FKey F22 = FKey("F22", 0x137);
    inline static const FKey F23 = FKey("F23", 0x138);
    inline static const FKey F24 = FKey("F24", 0x139);

    /* -----------------------------------------------------------------------
       Keyboard — Numpad
    ----------------------------------------------------------------------- */

    inline static const FKey NumPad0 = FKey("NumPad 0", 0x140);
    inline static const FKey NumPad1 = FKey("NumPad 1", 0x141);
    inline static const FKey NumPad2 = FKey("NumPad 2", 0x142);
    inline static const FKey NumPad3 = FKey("NumPad 3", 0x143);
    inline static const FKey NumPad4 = FKey("NumPad 4", 0x144);
    inline static const FKey NumPad5 = FKey("NumPad 5", 0x145);
    inline static const FKey NumPad6 = FKey("NumPad 6", 0x146);
    inline static const FKey NumPad7 = FKey("NumPad 7", 0x147);
    inline static const FKey NumPad8 = FKey("NumPad 8", 0x148);
    inline static const FKey NumPad9 = FKey("NumPad 9", 0x149);
    inline static const FKey NumPadDecimal = FKey("NumPad Decimal", 0x14A);
    inline static const FKey NumPadDivide = FKey("NumPad Divide", 0x14B);
    inline static const FKey NumPadMultiply = FKey("NumPad Multiply", 0x14C);
    inline static const FKey NumPadSubtract = FKey("NumPad Subtract", 0x14D);
    inline static const FKey NumPadAdd = FKey("NumPad Add", 0x14E);
    inline static const FKey NumPadEnter = FKey("NumPad Enter", 0x14F);

    /*---------------------*/
    /* -- Modifier Keys -- */
    /*---------------------*/

    inline static const FKey LeftShift = FKey("Left Shift", 0x154);
    inline static const FKey LeftControl = FKey("Left Control", 0x155);
    inline static const FKey LeftAlt = FKey("Left Alt", 0x156);
    inline static const FKey LeftSuper = FKey("Left Super", 0x157);
    inline static const FKey RightShift = FKey("Right Shift", 0x158);
    inline static const FKey RightControl = FKey("Right Control", 0x159);
    inline static const FKey RightAlt = FKey("Right Alt", 0x15A);
    inline static const FKey RightSuper = FKey("Right Super", 0x15B);

    /*---------------------*/
    /* -- Mouse Buttons -- */
    /*---------------------*/

    // Deliberately squats on the low 0x0-0x4 range, which no keyboard key above uses
    // (the lowest keyboard code is Space at 0x20) -- this is how mouse buttons and keyboard
    // keys safely share a single CKey::GetKeyFromKeyCode lookup map without colliding.
    inline static const FKey LeftMouseButton = FKey("Left Mouse Button", 0x0);
    inline static const FKey RightMouseButton = FKey("Right Mouse Button", 0x1);
    inline static const FKey MiddleMouseButton = FKey("Middle Mouse Button", 0x2);
    inline static const FKey ThumbMouseButtonOne = FKey("Thumb Mouse Button One", 0x3);
    inline static const FKey ThumbMouseButtonTwo = FKey("Thumb Mouse Button Two", 0x4);

    static void InitializeKeys();
private:
    static void AddKeyToMap(const FKey& Key);
};

inline FKey FKey::GetKeyFromKeyCode(uint32 KeyCode)
{
    const auto It = m_KeyCodeToKeyMap.find(KeyCode);

    return (It != m_KeyCodeToKeyMap.end()) ? It->second : FKey {};
}
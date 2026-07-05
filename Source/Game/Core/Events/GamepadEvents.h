#pragma once

#include "Event.h"

#include <sstream>

class CGamepadButtonEvent : public IEvent
{
public:
    uint32 GetGamepadID() const { return m_GamepadID; }
    EGamepadButton GetButton() const { return m_Button; }

    SET_CLASS_EVENT_CATEGORY(InputCategory | GamepadCategory)
protected:
    CGamepadButtonEvent(uint32 GamepadID, EGamepadButton Button)
        : m_GamepadID(GamepadID), m_Button(Button) {}

    uint32 m_GamepadID;
    EGamepadButton m_Button;
};

class CGamepadButtonPressedEvent final : public CGamepadButtonEvent
{
public:
    CGamepadButtonPressedEvent(uint32 GamepadID, EGamepadButton Button, uint64 InTimestampNs)
        : CGamepadButtonEvent(GamepadID, Button)
    {
        TimestampNs = InTimestampNs;
    }

    std::string ToString() const override
    {
        std::stringstream StringStream;
        StringStream << "GamepadButtonPressedEvent (Gamepad " << m_GamepadID << "): " << static_cast<int32>(m_Button)  << " (Timestamp: " << TimestampNs << "ns)";
        
        return StringStream.str();
    }

    SET_CLASS_EVENT_TYPE(GamepadButtonPressed)
};

class CGamepadButtonReleasedEvent final : public CGamepadButtonEvent
{
public:
    CGamepadButtonReleasedEvent(uint32 GamepadID, EGamepadButton Button, uint64 InTimestampNs)
        : CGamepadButtonEvent(GamepadID, Button)
    {
        TimestampNs = InTimestampNs;
    }

    std::string ToString() const override
    {
        std::stringstream StringStream;
        StringStream << "GamepadButtonReleasedEvent (Gamepad " << m_GamepadID << "): " << static_cast<int32>(m_Button)  << " (Timestamp: " << TimestampNs << "ns)";
        
        return StringStream.str();
    }

    SET_CLASS_EVENT_TYPE(GamepadButtonReleased)
};

class CGamepadConnectedEvent final : public IEvent
{
public:
    explicit CGamepadConnectedEvent(uint32 GamepadID)
        : m_GamepadID(GamepadID) {}

    uint32 GetGamepadID() const { return m_GamepadID; }

    std::string ToString() const override
    {
        std::stringstream Stream;
        Stream << "GamepadConnectedEvent: " << m_GamepadID;
        return Stream.str();
    }

    SET_CLASS_EVENT_TYPE(GamepadConnected)
    SET_CLASS_EVENT_CATEGORY(InputCategory | GamepadCategory)
private:
    uint32 m_GamepadID;
};

class CGamepadDisconnectedEvent final : public IEvent
{
public:
    explicit CGamepadDisconnectedEvent(uint32 GamepadID)
        : m_GamepadID(GamepadID) {}

    uint32 GetGamepadID() const { return m_GamepadID; }

    std::string ToString() const override
    {
        std::stringstream Stream;
        Stream << "GamepadDisconnectedEvent: " << m_GamepadID;
        return Stream.str();
    }

    SET_CLASS_EVENT_TYPE(GamepadDisconnected)
    SET_CLASS_EVENT_CATEGORY(InputCategory | GamepadCategory)
private:
    uint32 m_GamepadID;
};
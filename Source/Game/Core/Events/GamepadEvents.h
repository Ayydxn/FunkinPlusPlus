#pragma once

#include "Event.h"

#include <sstream>

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
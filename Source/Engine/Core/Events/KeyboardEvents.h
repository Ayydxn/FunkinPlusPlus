#pragma once

#include "Event.h"

#include <sstream>

class CKeyboardEvent : public IEvent
{
public:
    uint32 GetKey() const { return m_Key; }

    SET_CLASS_EVENT_CATEGORY(InputCategory | KeyboardCategory)
protected:
    explicit CKeyboardEvent(const uint32& Key)
        : m_Key(Key) {}

    uint32 m_Key;
};

class CKeyPressedEvent final : public CKeyboardEvent
{
public:
    CKeyPressedEvent(uint32 Key, bool bInIsHeldDown, uint64 InTimestampNs)
        : CKeyboardEvent(Key), bIsHeldDown(bInIsHeldDown)
    {
        TimestampNs = InTimestampNs;
    }

    bool IsKeyHeldDown() const { return bIsHeldDown; }

    std::string ToString() const override
    {
        std::stringstream Stream;
        Stream << "KeyPressedEvent: " << m_Key << " (Held: " << (bIsHeldDown ? "True" : "False") << ", Timestamp: " << TimestampNs << "ns)";

        return Stream.str();
    }

    SET_CLASS_EVENT_TYPE(KeyPressed)
private:
    bool bIsHeldDown;
};

class CKeyReleasedEvent final : public CKeyboardEvent
{
public:
    CKeyReleasedEvent(uint32 Key, uint64 InTimestampNs)
        : CKeyboardEvent(Key)
    {
        TimestampNs = InTimestampNs;
    }

    std::string ToString() const override
    {
        std::stringstream Stream;
        Stream << "KeyReleasedEvent: " << m_Key << " (Timestamp: " << TimestampNs << "ns)";
        return Stream.str();
    }

    SET_CLASS_EVENT_TYPE(KeyReleased)
};

class CKeyTypedEvent final : public CKeyboardEvent
{
public:
    CKeyTypedEvent(uint32 KeyCode, uint64 InTimestampNs)
        : CKeyboardEvent(-1), m_KeyCode(KeyCode)
    {
        TimestampNs = InTimestampNs;
    }

    std::string ToString() const override
    {
        std::stringstream Stream;
        Stream << "KeyTypedEvent: " << m_KeyCode;
        return Stream.str();
    }

    SET_CLASS_EVENT_TYPE(KeyTyped)

private:
    uint32 m_KeyCode;
};

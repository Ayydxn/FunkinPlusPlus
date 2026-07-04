#pragma once

#include "Event.h"
#include "Input/CoreInputTypes.h"

#include <sstream>

class CKeyboardEvent : public IEvent
{
public:
    const FKey& GetKey() const { return m_Key; }

    SET_CLASS_EVENT_CATEGORY(InputCategory | KeyboardCategory)
protected:
    explicit CKeyboardEvent(const FKey& Key)
        : m_Key(Key) {}

    FKey m_Key;
};

class CKeyPressedEvent final : public CKeyboardEvent
{
public:
    CKeyPressedEvent(const FKey& Key, bool bInIsHeldDown, uint64 InTimestampNs)
        : CKeyboardEvent(Key), bIsHeldDown(bInIsHeldDown)
    {
        TimestampNs = InTimestampNs;
    }

    bool IsKeyHeldDown() const { return bIsHeldDown; }

    std::string ToString() const override
    {
        std::stringstream Stream;
        Stream << "KeyPressedEvent: " << m_Key.GetDisplayName() << " (Held: " << (bIsHeldDown ? "True" : "False") << ", Timestamp: " << TimestampNs << "ns)";

        return Stream.str();
    }

    SET_CLASS_EVENT_TYPE(KeyPressed)
private:
    bool bIsHeldDown;
};

class CKeyReleasedEvent final : public CKeyboardEvent
{
public:
    CKeyReleasedEvent(const FKey& Key, uint64 InTimestampNs)
        : CKeyboardEvent(Key)
    {
        TimestampNs = InTimestampNs;
    }

    std::string ToString() const override
    {
        std::stringstream Stream;
        Stream << "KeyReleasedEvent: " << m_Key.GetDisplayName() << " (Timestamp: " << TimestampNs << "ns)";
        return Stream.str();
    }

    SET_CLASS_EVENT_TYPE(KeyReleased)
};

class CKeyTypedEvent final : public IEvent
{
public:
    CKeyTypedEvent(uint32 Codepoint, uint64 InTimestampNs)
        : m_Codepoint(Codepoint)
    {
        TimestampNs = InTimestampNs;
    }

    std::string ToString() const override
    {
        std::stringstream Stream;
        Stream << "KeyTypedEvent: " << m_Codepoint;
        return Stream.str();
    }

    SET_CLASS_EVENT_TYPE(KeyTyped)
    SET_CLASS_EVENT_CATEGORY(InputCategory | KeyboardCategory)
private:
    uint32 m_Codepoint;
};

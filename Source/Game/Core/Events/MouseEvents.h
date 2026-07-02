#pragma once

#include "Event.h"

#include <sstream>

class CMouseButtonEvent : public IEvent
{
public:
    uint32 GetMouseButton() const { return m_MouseButton; }

    SET_CLASS_EVENT_CATEGORY(InputCategory | MouseCategory | MouseButtonCategory)
protected:
    CMouseButtonEvent(const uint32& MouseButton)
        : m_MouseButton(MouseButton) {}

    uint32 m_MouseButton;
};

class CMouseButtonPressedEvent final : public CMouseButtonEvent
{
public:
    CMouseButtonPressedEvent(uint32 MouseButton, uint64 InTimestampNs)
        : CMouseButtonEvent(MouseButton)
    {
        TimestampNs = InTimestampNs;
    }

    std::string ToString() const override
    {
        std::stringstream StringStream;
        StringStream << "MouseButtonPressedEvent: " << m_MouseButton << " (Timestamp: " << TimestampNs << "ns)";
        return StringStream.str();
    }

    SET_CLASS_EVENT_TYPE(MouseButtonPressed)
};

class CMouseButtonReleasedEvent final : public CMouseButtonEvent
{
public:
    CMouseButtonReleasedEvent(uint32 MouseButton, uint64 InTimestampNs)
        : CMouseButtonEvent(MouseButton)
    {
        TimestampNs = InTimestampNs;
    }

    std::string ToString() const override
    {
        std::stringstream StringStream;
        StringStream << "MouseButtonReleasedEvent: " << m_MouseButton << " (Timestamp: " << TimestampNs << "ns)";
        return StringStream.str();
    }

    SET_CLASS_EVENT_TYPE(MouseButtonReleased)
};

class CMouseMovedEvent final : public IEvent
{
public:
    CMouseMovedEvent(const float XPosition, const float YPosition)
        : m_XPosition(XPosition), m_YPosition(YPosition) {}

    float GetXPosition() const { return m_XPosition; }
    float GetYPosition() const { return m_YPosition; }

    std::string ToString() const override
    {
        std::stringstream StringStream;
        StringStream << "MouseMovedEvent: (X: " << m_XPosition << ", Y: " << m_YPosition << ")";
        return StringStream.str();
    }

    SET_CLASS_EVENT_TYPE(MouseMoved)
    SET_CLASS_EVENT_CATEGORY(InputCategory | MouseCategory)
private:
    float m_XPosition, m_YPosition;
};

class CMouseScrolledEvent final : public IEvent
{
public:
    CMouseScrolledEvent(const float XOffset, const float YOffset)
        : m_XOffset(XOffset), m_YOffset(YOffset) {}

    float GetXOffset() const { return m_XOffset; }
    float GetYOffset() const { return m_YOffset; }

    std::string ToString() const override
    {
        std::stringstream StringStream;
        StringStream << "MouseScrolledEvent: (X: " << m_XOffset << ", Y: " << m_YOffset << ")";
        return StringStream.str();
    }

    SET_CLASS_EVENT_TYPE(MouseScrolled)
    SET_CLASS_EVENT_CATEGORY(InputCategory | MouseCategory)
private:
    float m_XOffset, m_YOffset;
};
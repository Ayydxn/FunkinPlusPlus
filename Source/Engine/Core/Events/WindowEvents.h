#pragma once

#include "Event.h"

#include <sstream>

class CWindowCloseEvent final : public IEvent
{
public:
    CWindowCloseEvent() = default;

    std::string ToString() const override { return "WindowCloseEvent"; }

    SET_CLASS_EVENT_TYPE(WindowClose)
    SET_CLASS_EVENT_CATEGORY(EngineCategory)
};

class CWindowMinimizeEvent final : public IEvent
{
public:
    explicit CWindowMinimizeEvent(bool bInIsMinimized) : bIsMinimized(bInIsMinimized) {}

    bool IsWindowMinimized() const { return bIsMinimized; }

    std::string ToString() const override
    {
        std::stringstream Stream;
        Stream << "WindowMinimizeEvent: " << (bIsMinimized ? "Minimized" : "Restored");
        return Stream.str();
    }

    SET_CLASS_EVENT_TYPE(WindowMinimize)
    SET_CLASS_EVENT_CATEGORY(EngineCategory)

private:
    bool bIsMinimized = false;
};

class CWindowResizeEvent final : public IEvent
{
public:
    CWindowResizeEvent(uint32 InWidth, uint32 InHeight)
        : m_Width(InWidth), m_Height(InHeight) {}

    uint32 GetWidth()  const { return m_Width; }
    uint32 GetHeight() const { return m_Height; }

    std::string ToString() const override
    {
        std::stringstream Stream;
        Stream << "WindowResizeEvent: (" << m_Width << "x" << m_Height << ")";
        return Stream.str();
    }

    SET_CLASS_EVENT_TYPE(WindowResize)
    SET_CLASS_EVENT_CATEGORY(EngineCategory)

private:
    uint32 m_Width  = 0;
    uint32 m_Height = 0;
};

class CWindowFocusEvent final : public IEvent
{
public:
    CWindowFocusEvent() = default;
    std::string ToString() const override { return "WindowFocusEvent"; }

    SET_CLASS_EVENT_TYPE(WindowFocus)
    SET_CLASS_EVENT_CATEGORY(EngineCategory)
};

class CWindowLostFocusEvent final : public IEvent
{
public:
    CWindowLostFocusEvent() = default;
    std::string ToString() const override { return "WindowLostFocusEvent"; }

    SET_CLASS_EVENT_TYPE(WindowLostFocus)
    SET_CLASS_EVENT_CATEGORY(EngineCategory)
};
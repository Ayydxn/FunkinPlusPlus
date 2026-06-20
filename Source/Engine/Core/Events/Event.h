#pragma once

#include "Core/CoreTypes.h"

#include <functional>
#include <string>

enum class EEventType : uint8
{
    Unknown,

    KeyPressed, KeyReleased, KeyTyped,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
    WindowClose, WindowMinimize, WindowResize, WindowFocus, WindowLostFocus,
    EnginePreInitialize, EngineInitialize, EngineUpdate, EngineFixedUpdate, EngineRender, EngineShutdown
};

enum EEventCategory : uint8
{
    EngineCategory      = 1u << 0,
    InputCategory       = 1u << 1,
    KeyboardCategory    = 1u << 2,
    MouseCategory       = 1u << 3,
    MouseButtonCategory = 1u << 4
};

#define SET_CLASS_EVENT_TYPE(Type) static EEventType GetStaticType() { return EEventType::Type; }\
                                   virtual EEventType GetEventType() const override { return GetStaticType(); }\
                                   virtual const char* GetName() const override { return #Type; }

#define SET_CLASS_EVENT_CATEGORY(Category) virtual int32 GetCategoryFlags() const override { return EEventCategory::Category; }

class IEvent
{
public:
    virtual ~IEvent() = default;

    virtual EEventType GetEventType() const = 0;
    virtual const char* GetName() const = 0;
    virtual int GetCategoryFlags() const = 0;
    virtual std::string ToString() const { return GetName(); }

    bool IsInCategory(EEventCategory EventCategory) const
    {
        return GetCategoryFlags() & EventCategory;
    }

    uint64 TimestampNs = 0;
    bool bIsHandled = false;
};

class CEventDispatcher
{
    template<typename EventClass>
    using EventFunction = std::function<bool(EventClass&)>;
public:
    explicit CEventDispatcher(IEvent& Event)
        : m_Event(Event) {}

    template<typename EventClass>
    bool Dispatch(EventFunction<EventClass> EventFunc)
    {
        if (m_Event.GetEventType() == EventClass::GetStaticType() && !m_Event.bIsHandled)
        {
            m_Event.bIsHandled = EventFunc(*static_cast<EventClass*>(&m_Event));
            return true;
        }

        return false;
    }
private:
    IEvent& m_Event;
};

inline std::ostream& operator<<(std::ostream& OutStream, const IEvent& Event)
{
    return OutStream << Event.ToString();
}

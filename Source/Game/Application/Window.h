#pragma once

#include "Core/Events/Event.h"

#include <string>
#include <utility>

enum class EWindowMode : uint8
{
    Windowed,
    WindowedFullscreen,
    Fullscreen
};

struct FWindowSpecification
{
    std::string Title = "Friday Night Funkin'++";
    uint32 Width = 1600, Height = 900;
    EWindowMode WindowMode = EWindowMode::Windowed;
    bool bEnableVSync = true;
    bool bEnableDecoration = true;
    bool bEnableResizing = true;
};

struct FNativeWindowHandle
{
    enum class EPlatform : uint8
    {
        Win32,
        Cocoa,
        X11,
        Wayland
    } Platform;

    union
    {
        struct { void* Hwnd; void* HInstance; } Win32;
        struct { void* NsWindow; } Cocoa;
        struct { unsigned long Window; void* Display; } X11;
        struct { void* Surface; void* Display; } Wayland;
    };
};

struct SDL_Window;
union SDL_Event;

class CWindow
{
    using FEventCallbackFunction = std::function<void(IEvent&)>;
public:
    CWindow() = default;
    ~CWindow();
    
    CWindow(const CWindow&) = delete;
    CWindow& operator=(const CWindow&) = delete;
    
    bool Initialize(const FWindowSpecification& Specification);
    void Destroy();
    
    uint32 GetWidth() const { return m_WindowState.Width; }
    uint32 GetHeight() const { return m_WindowState.Height; }
    std::pair<uint32, uint32> GetSize() const { return { m_WindowState.Width, m_WindowState.Height }; }
    
    void HandleNativeEvent(const SDL_Event& NativeEvent, uint64 CaptureTimestampNs);

    std::pair<int32, int32> GetPosition() const;

    void Maximize() const;
    void CenterOnScreen() const;
    
    void SetEventCallbackFunction(FEventCallbackFunction EventCallbackFunction) { m_WindowState.EventCallbackFunction = std::move(EventCallbackFunction); }

    bool WantsVSync() const { return m_WindowState.bWantsVSync; }

    void SetWindowMode(EWindowMode NewWindowMode);
    void EnableResizing(bool bEnableResizing);

    const std::string& GetTitle() const { return m_WindowState.Title; }
    void SetTitle(const std::string& Title);
    
    const FWindowSpecification& GetSpecification() const { return m_Specification; }

    float GetDisplayRefreshRate() const;
    
    FNativeWindowHandle GetNativeHandle() const;
    uint32 GetNativeWindowID() const { return m_WindowID; }
protected:
    struct CWindowState
    {
        std::string Title;
        uint32 Width = 0, Height = 0;
        EWindowMode WindowMode = EWindowMode::Windowed;
        bool bWantsVSync = true;
        bool bIsDecorationEnabled = true;
        bool bIsResizingEnabled = true;

        FEventCallbackFunction EventCallbackFunction;
    };
protected:
    CWindowState m_WindowState = {};
    FWindowSpecification m_Specification = {};
private:
    SDL_Window* m_WindowHandle = nullptr;
    uint32 m_WindowID = 0;
};

#include "FunkinPCH.h"
#include "Window.h"
#include "Events/KeyboardEvents.h"
#include "Events/MouseEvents.h"
#include "Events/WindowEvents.h"

#include <SDL3/SDL.h>

CWindow::~CWindow()
{
    Destroy();
}

bool CWindow::Initialize(const FWindowSpecification& Specification)
{
    LOG_INFO_TAG("Core", "Creating window '{}'... ({} x {})", Specification.Title, Specification.Width, Specification.Height);
    
    m_Specification = Specification;

    m_WindowState.Title = Specification.Title;
    m_WindowState.Width = Specification.Width;
    m_WindowState.Height = Specification.Height;
    m_WindowState.WindowMode = Specification.WindowMode;
    m_WindowState.bWantsVSync = Specification.bEnableVSync;
    m_WindowState.bIsDecorationEnabled = Specification.bEnableDecoration;
    m_WindowState.bIsResizingEnabled = Specification.bEnableResizing;
    
    SDL_WindowFlags WindowFlags = SDL_WINDOW_INPUT_FOCUS;

    if (!Specification.bEnableDecoration)
        WindowFlags |= SDL_WINDOW_BORDERLESS;

    if (Specification.bEnableResizing)
        WindowFlags |= SDL_WINDOW_RESIZABLE;

    m_WindowHandle = SDL_CreateWindow(Specification.Title.c_str(), static_cast<int32>(Specification.Width), static_cast<int32>(Specification.Height), WindowFlags);
    verifyFunkinf(m_WindowHandle, "Failed to create window '{}'! ({})", m_Specification.Title, SDL_GetError())
    
    m_WindowID = SDL_GetWindowID(m_WindowHandle);
    
    SetWindowMode(m_Specification.WindowMode);
    
    return true;
}

void CWindow::Destroy()
{
    if (m_WindowHandle)
    {
        SDL_DestroyWindow(m_WindowHandle);
        
        m_WindowHandle = nullptr;
        m_WindowID = 0;
    }
}

void CWindow::HandleNativeEvent(const SDL_Event& NativeEvent, uint64 CaptureTimestampNs)
{
    if (!m_WindowState.EventCallbackFunction)
        return;
    
    switch (NativeEvent.type)
    {
        case SDL_EVENT_KEY_DOWN:
        {
            CKeyPressedEvent KeyPressedEvent(NativeEvent.key.key, NativeEvent.key.repeat, CaptureTimestampNs);
            m_WindowState.EventCallbackFunction(KeyPressedEvent);
            
            break;
        }
            
        case SDL_EVENT_KEY_UP:
        {
            CKeyReleasedEvent KeyReleasedEvent(NativeEvent.key.key, CaptureTimestampNs);
            m_WindowState.EventCallbackFunction(KeyReleasedEvent);
            
            break;
        }
            
        case SDL_EVENT_TEXT_INPUT:
        {
            // SDL_EVENT_TEXT_INPUT gives UTF-8. Fire one KeyTyped per codepoint.
            // For the common case of ASCII the first byte is the codepoint.
            const uint32 Codepoint = static_cast<uint8>(NativeEvent.text.text[0]);
                
            CKeyTypedEvent KeyTypedEvent(Codepoint, CaptureTimestampNs);
            m_WindowState.EventCallbackFunction(KeyTypedEvent);
            break;
        }
            
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            CMouseButtonPressedEvent MouseButtonPressedEvent(NativeEvent.button.button, CaptureTimestampNs);
            m_WindowState.EventCallbackFunction(MouseButtonPressedEvent);
            
            break;
        }
            
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            CMouseButtonReleasedEvent MouseButtonReleasedEvent(NativeEvent.button.button, CaptureTimestampNs);
            m_WindowState.EventCallbackFunction(MouseButtonReleasedEvent);
            
            break;
        }
            
        case SDL_EVENT_MOUSE_MOTION:
        {
            CMouseMovedEvent MouseMovedEvent(NativeEvent.motion.x, NativeEvent.motion.y);
            m_WindowState.EventCallbackFunction(MouseMovedEvent);
            
            break;
        }
            
        case SDL_EVENT_MOUSE_WHEEL:
        {
            CMouseScrolledEvent MouseScrolledEvent(NativeEvent.wheel.x, NativeEvent.wheel.y);
            m_WindowState.EventCallbackFunction(MouseScrolledEvent);
            
            break;
        }
            
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        {
            CWindowCloseEvent WindowCloseEvent(NativeEvent.window.windowID);
            WindowCloseEvent.TimestampNs = CaptureTimestampNs;
            
            m_WindowState.EventCallbackFunction(WindowCloseEvent);
            break;
        }
            
        case SDL_EVENT_WINDOW_MINIMIZED:
        {
            CWindowMinimizeEvent WindowMinimizeEvent(true);
            m_WindowState.EventCallbackFunction(WindowMinimizeEvent);
            
            break;
        }

        case SDL_EVENT_WINDOW_RESTORED:
        {
            CWindowMinimizeEvent WindowMinimizeEvent(false);
            m_WindowState.EventCallbackFunction(WindowMinimizeEvent);
            
            break;
        }

        case SDL_EVENT_WINDOW_RESIZED:
        {
            m_WindowState.Width = static_cast<uint32>(NativeEvent.window.data1);
            m_WindowState.Height = static_cast<uint32>(NativeEvent.window.data2);

            CWindowResizeEvent WindowResizeEvent(NativeEvent.window.data1, NativeEvent.window.data2);
            m_WindowState.EventCallbackFunction(WindowResizeEvent);
            
            break;
        }

        case SDL_EVENT_WINDOW_FOCUS_GAINED:
        {
            CWindowFocusEvent WindowFocusEvent;
            m_WindowState.EventCallbackFunction(WindowFocusEvent);
            
            break;
        }

        case SDL_EVENT_WINDOW_FOCUS_LOST:
        {
            CWindowLostFocusEvent WindowLostFocusEvent;
            m_WindowState.EventCallbackFunction(WindowLostFocusEvent);
            
            break;
        }
            
        default: break;
    }
}

std::pair<int32, int32> CWindow::GetPosition() const
{
    int32 XPosition, YPosition;
    SDL_GetWindowPosition(m_WindowHandle, &XPosition, &YPosition);
    
    return { XPosition, YPosition };
}

void CWindow::Maximize() const
{
    SDL_MaximizeWindow(m_WindowHandle);
}

void CWindow::CenterOnScreen() const
{
    SDL_SetWindowPosition(m_WindowHandle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void CWindow::SetWindowMode(EWindowMode NewWindowMode)
{
    m_WindowState.WindowMode = NewWindowMode;

    switch (NewWindowMode)
    {
        case EWindowMode::Windowed:
        {
            SDL_SetWindowFullscreen(m_WindowHandle, false);
            
            break;
        }

        case EWindowMode::WindowedFullscreen:
        {
            SDL_SetWindowFullscreen(m_WindowHandle, false);

            const SDL_DisplayID WindowDisplay = SDL_GetDisplayForWindow(m_WindowHandle);
            SDL_Rect DisplayBounds;
            SDL_GetDisplayBounds(WindowDisplay, &DisplayBounds);

            SDL_SetWindowBordered(m_WindowHandle, false);
            SDL_SetWindowPosition(m_WindowHandle, DisplayBounds.x, DisplayBounds.y);
            SDL_SetWindowSize(m_WindowHandle, DisplayBounds.w, DisplayBounds.h);
            
            break;
        }

        case EWindowMode::Fullscreen:
        {
            SDL_SetWindowFullscreen(m_WindowHandle, true);
            
            break;
        }
    }
}

void CWindow::EnableResizing(bool bEnableResizing)
{
    SDL_SetWindowResizable(m_WindowHandle, bEnableResizing);
    
    m_WindowState.bIsResizingEnabled = bEnableResizing;
}

void CWindow::SetTitle(const std::string& Title)
{
    SDL_SetWindowTitle(m_WindowHandle, Title.c_str());
    
    m_WindowState.Title = Title;
}

FNativeWindowHandle CWindow::GetNativeHandle() const
{
    FNativeWindowHandle NativeWindowHandle;
    
    #ifdef FUNKIN_PLATFORM_WIN64
        const SDL_PropertiesID WindowProperties = SDL_GetWindowProperties(m_WindowHandle);
    
        NativeWindowHandle.Platform = FNativeWindowHandle::EPlatform::Win32;
        NativeWindowHandle.Win32.Hwnd = SDL_GetPointerProperty(WindowProperties, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
        NativeWindowHandle.Win32.HInstance = SDL_GetPointerProperty(WindowProperties, SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, nullptr);
    #endif
    
    return NativeWindowHandle;
}

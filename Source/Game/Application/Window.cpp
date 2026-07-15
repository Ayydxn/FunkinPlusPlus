#include "FunkinPCH.h"
#include "Window.h"
#include "Events/KeyboardEvents.h"
#include "Events/MouseEvents.h"
#include "Events/WindowEvents.h"

#include <SDL3/SDL.h>

namespace
{
    uint32 SDLKeycodeToFunkinKeyCode(SDL_Keycode Key)
    {
        // Letters: SDL lowercase -> engine uppercase ASCII
        if (Key >= SDLK_A && Key <= SDLK_Z)
            return Key - 32; // 'a'(97) -> 'A'(65)
    
        // Printable ASCII that match directly (space=32, digits=48-57, punctuation)
        if (Key == SDLK_SPACE) return 0x20;
        if (Key == SDLK_APOSTROPHE) return 0x27;
        if (Key == SDLK_COMMA) return 0x2C;
        if (Key == SDLK_MINUS) return 0x2D;
        if (Key == SDLK_PERIOD) return 0x2E;
        if (Key == SDLK_SLASH) return 0x2F;
        if (Key >= SDLK_0 && Key <= SDLK_9) return Key; // 0x30-0x39
        if (Key == SDLK_SEMICOLON) return 0x3B;
        if (Key == SDLK_EQUALS) return 0x3D;
        if (Key == SDLK_LEFTBRACKET) return 0x5B;
        if (Key == SDLK_BACKSLASH) return 0x5C;
        if (Key == SDLK_RIGHTBRACKET) return 0x5D;
        if (Key == SDLK_GRAVE) return 0x60;
    
        // Navigation / function keys -> engine 0x100+ range (mirrors GLFW_KEY_* values)
        switch (Key)
        {
            case SDLK_ESCAPE: return 0x100;
            case SDLK_RETURN: return 0x101;
            case SDLK_TAB: return 0x102;
            case SDLK_BACKSPACE: return 0x103;
            case SDLK_INSERT: return 0x104;
            case SDLK_DELETE: return 0x105;
            case SDLK_RIGHT: return 0x106;
            case SDLK_LEFT: return 0x107;
            case SDLK_DOWN: return 0x108;
            case SDLK_UP: return 0x109;
            case SDLK_PAGEUP: return 0x10A;
            case SDLK_PAGEDOWN: return 0x10B;
            case SDLK_HOME: return 0x10C;
            case SDLK_END: return 0x10D;
            case SDLK_CAPSLOCK: return 0x118;
            case SDLK_SCROLLLOCK: return 0x119;
            case SDLK_NUMLOCKCLEAR: return 0x11A;
            case SDLK_PRINTSCREEN: return 0x11B;
            case SDLK_PAUSE: return 0x11C;
            case SDLK_F1: return 0x122;
            case SDLK_F2: return 0x123;
            case SDLK_F3: return 0x124;
            case SDLK_F4: return 0x125;
            case SDLK_F5: return 0x126;
            case SDLK_F6: return 0x127;
            case SDLK_F7: return 0x128;
            case SDLK_F8: return 0x129;
            case SDLK_F9: return 0x12A;
            case SDLK_F10: return 0x12B;
            case SDLK_F11: return 0x12C;
            case SDLK_F12: return 0x12D;
            case SDLK_F13: return 0x12E;
            case SDLK_F14: return 0x12F;
            case SDLK_F15: return 0x130;
            case SDLK_F16: return 0x131;
            case SDLK_F17: return 0x132;
            case SDLK_F18: return 0x133;
            case SDLK_F19: return 0x134;
            case SDLK_F20: return 0x135;
            case SDLK_F21: return 0x136;
            case SDLK_F22: return 0x137;
            case SDLK_F23: return 0x138;
            case SDLK_F24: return 0x139;
            case SDLK_KP_0: return 0x140;
            case SDLK_KP_1: return 0x141;
            case SDLK_KP_2: return 0x142;
            case SDLK_KP_3: return 0x143;
            case SDLK_KP_4: return 0x144;
            case SDLK_KP_5: return 0x145;
            case SDLK_KP_6: return 0x146;
            case SDLK_KP_7: return 0x147;
            case SDLK_KP_8: return 0x148;
            case SDLK_KP_9: return 0x149;
            case SDLK_KP_PERIOD: return 0x14A;
            case SDLK_KP_DIVIDE: return 0x14B;
            case SDLK_KP_MULTIPLY: return 0x14C;
            case SDLK_KP_MINUS: return 0x14D;
            case SDLK_KP_PLUS: return 0x14E;
            case SDLK_KP_ENTER: return 0x14F;
            case SDLK_LSHIFT: return 0x154;
            case SDLK_LCTRL: return 0x155;
            case SDLK_LALT: return 0x156;
            case SDLK_LGUI: return 0x157;
            case SDLK_RSHIFT: return 0x158;
            case SDLK_RCTRL: return 0x159;
            case SDLK_RALT: return 0x15A;
            case SDLK_RGUI: return 0x15B;
            default: return FKey::InvalidKeyCode;
        }
    }
    
    FKey SDLKeycodeToFunkinKey(SDL_Keycode Key)
    {
        return FKey::GetKeyFromKeyCode(SDLKeycodeToFunkinKeyCode(Key));
    }

    FKey SDLMouseButtonToKey(uint8 SDLButton)
    {
        // SDL3 buttons are 1-based (LEFT=1, MIDDLE=2, RIGHT=3, X1=4, X2=5).
        // Engine mouse button codes are 0-based, so we subtract it by 1 to prevent mismatches and errors because a keycode with 5 as a value doesn't exist.
        return FKey::GetKeyFromKeyCode(static_cast<uint32>(SDLButton - 1));
    }
}

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
            CKeyPressedEvent KeyPressedEvent(SDLKeycodeToFunkinKey(NativeEvent.key.key), NativeEvent.key.repeat, CaptureTimestampNs);
            m_WindowState.EventCallbackFunction(KeyPressedEvent);
            
            break;
        }
            
        case SDL_EVENT_KEY_UP:
        {
            CKeyReleasedEvent KeyReleasedEvent(SDLKeycodeToFunkinKey(NativeEvent.key.key), CaptureTimestampNs);
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
            CMouseButtonPressedEvent MouseButtonPressedEvent(SDLKeycodeToFunkinKey(NativeEvent.button.button), CaptureTimestampNs);
            m_WindowState.EventCallbackFunction(MouseButtonPressedEvent);
            
            break;
        }
            
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            CMouseButtonReleasedEvent MouseButtonReleasedEvent(SDLKeycodeToFunkinKey(NativeEvent.button.button), CaptureTimestampNs);
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

            CWindowResizeEvent WindowResizeEvent(NativeEvent.window.data1, NativeEvent.window.data2, NativeEvent.window.windowID);
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

float CWindow::GetDisplayRefreshRate() const
{
    const SDL_DisplayID WindowDisplayID = SDL_GetDisplayForWindow(m_WindowHandle);
    if (WindowDisplayID == 0)
        return 0.0f;

    const SDL_DisplayMode* DisplayMode = SDL_GetCurrentDisplayMode(WindowDisplayID);
    if (!DisplayMode)
        return 0.0f;

    return DisplayMode->refresh_rate;
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

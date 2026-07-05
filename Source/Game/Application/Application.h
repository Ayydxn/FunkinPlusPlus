#pragma once

#include "Window.h"
#include "Engine/EngineContext.h"

union SDL_Event;

class CApplication
{
public:
    bool Initialize(CEngineContext& EngineContext, const FWindowSpecification& WindowSpecification = FWindowSpecification());
    void Shutdown();
    
    void PumpMessages();
    
    CWindow* MakeWindow(const FWindowSpecification& Specification);
    void DestroyWindow(const CWindow* Window);
    void DestroyWindowByID(uint32 WindowID);
    
    CWindow& GetMainWindow() { return *m_MainWindow; }
    const CWindow& GetMainWindow() const { return *m_MainWindow; }
private:
    // Returns true if a gamepad event was received and handled, false otherwise.
    bool HandleGamepadEvent(const SDL_Event& Event, uint64 CaptureTimestampNs) const;
private:
    std::unordered_map<uint32, std::unique_ptr<CWindow>> m_SecondaryWindows; // SDL Window ID -> CDesktopWindow instance
    std::unique_ptr<CWindow> m_MainWindow;
    
    CEngineContext* m_EngineContext = nullptr;
};

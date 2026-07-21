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
    
    CWindow& GetWindow() { return *m_Window; }
    const CWindow& GetWindow() const { return *m_Window; }
private:
    // Returns true if a gamepad event was received and handled, false otherwise.
    bool HandleGamepadEvent(const SDL_Event& Event, uint64 CaptureTimestampNs) const;
private:
    std::unique_ptr<CWindow> m_Window;
    
    CEngineContext* m_EngineContext = nullptr;
};

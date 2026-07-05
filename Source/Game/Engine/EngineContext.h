#pragma once

#include "FramePacer.h"
#include "Delegates/MulticastDelegate.h"
#include "Events/EventBroadcaster.h"
#include "Input/InputState.h"

class CEngineContext
{
public:
    CEngineContext() = default;

    CEngineContext(const CEngineContext&) = delete;
    CEngineContext& operator=(const CEngineContext&) = delete;
    
    void Initialize() { m_InputState.Initialize(m_EventBroadcaster); }
    void Shutdown() { m_InputState.Shutdown(m_EventBroadcaster); }

    CEventBroadcaster& GetEventBroadcaster() { return m_EventBroadcaster; }
    CInputState& GetInputState() { return m_InputState; }
    const FFrameStats& GetFrameStats() const { return m_FrameStats; }
private:
    void SetFrameStats(const FFrameStats& FrameStats) { m_FrameStats = FrameStats; }
private:
    CEventBroadcaster m_EventBroadcaster;
    CInputState m_InputState;
    FFrameStats m_FrameStats;
    
    friend class CFunkinLoop;
};

struct FEngineDelegates
{
    inline static TMulticastDelegate<> PreInitializeDelegate;
    inline static TMulticastDelegate<> InitializeDelegate;
    inline static TMulticastDelegate<> ShutdownDelegate;
};

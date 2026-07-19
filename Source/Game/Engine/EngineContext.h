#pragma once

#include "FramePacer.h"
#include "Delegates/MulticastDelegate.h"
#include "Events/EventBroadcaster.h"
#include "Input/InputState.h"
#include "Renderer/Renderer.h"
#include "RHICore/DynamicRHI.h"
#include "RHICore/RHIContext.h"

class CEngineContext
{
public:
    CEngineContext() = default;

    CEngineContext(const CEngineContext&) = delete;
    CEngineContext& operator=(const CEngineContext&) = delete;
    
    bool Initialize(ERHIBackend RHIBackend, uint32 WindowID, const FNativeWindowHandle& NativeWindowHandle, uint32 InitialWindowWidth, uint32 InitialWindowHeight,
        bool bRequestVSync);
    void Shutdown();
    
    bool RegisterWindow(uint32 WindowID, const FNativeWindowHandle& NativeWindowHandle, uint32 InitialWidth, uint32 InitialHeight, bool bRequestVSync) const;
    void UnregisterWindow(uint32 WindowID) const;
    
    static CEngineContext& GetInstance();

    IRHIContext& GetRHIContext() const { return *m_RHIContext; }
    CRenderer& GetRenderer() const { return *m_Renderer; }
    CEventBroadcaster& GetEventBroadcaster() { return m_EventBroadcaster; }
    CInputState& GetInputState() { return m_InputState; }
    const FFrameStats& GetFrameStats() const { return m_FrameStats; }
private:
    void SetFrameStats(const FFrameStats& FrameStats) { m_FrameStats = FrameStats; }
private:
    inline static CEngineContext* m_Instance = nullptr;
    
    std::unique_ptr<IRHIContext> m_RHIContext;
    std::unique_ptr<IDynamicRHI> m_DynamicRHI;
    std::unique_ptr<CRenderer> m_Renderer;
    
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

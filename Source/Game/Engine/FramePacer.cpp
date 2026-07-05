#include "FunkinPCH.h"
#include "FramePacer.h"

#include <SDL3/SDL.h>

#ifndef FUNKIN_PLATFORM_WIN64
    #include <time.h>
#endif

void CFramePacer::Initialize(double TickIntervalSeconds, double RenderIntervalSeconds)
{
    m_TickIntervalSeconds = TickIntervalSeconds;
    m_RenderIntervalSeconds = RenderIntervalSeconds;

    const double Now = GetNowSeconds();

    m_LastTickTimeSeconds = Now;
    m_LastRenderTimeSeconds = Now;
    m_StatsWindowStartSeconds = Now;
}

void CFramePacer::WaitForNextDeadline() const
{
    const double NextDeadlineSeconds = std::min(GetNextUpdateDueSeconds(), GetNextRenderDueSeconds());

    double Now = GetNowSeconds();
    const double TimeToWaitSeconds = NextDeadlineSeconds - Now;

    if (TimeToWaitSeconds <= 0.0)
        return;

    // Sleep for the bulk of the wait, leaving a margin to spin through for precision.
    if (TimeToWaitSeconds > m_SpinMarginSeconds)
        PlatformSleepSeconds(TimeToWaitSeconds - m_SpinMarginSeconds);

    // Spin the remainder - sleep is imprecise, this is what actually lands us on the deadline.
    while ((Now = GetNowSeconds()) < NextDeadlineSeconds)
    {
        // Intentionally empty - busy-wait for precision.
    }
}

bool CFramePacer::IsTickDue() const
{
    return GetNowSeconds() - m_LastTickTimeSeconds >= m_TickIntervalSeconds;
}

bool CFramePacer::IsRenderDue() const
{
    if (bRenderUncapped)
        return true;

    return GetNowSeconds() - m_LastRenderTimeSeconds >= m_RenderIntervalSeconds;
}

void CFramePacer::OnTickExecuted()
{
    m_LastTickTimeSeconds = GetNowSeconds();
    
    ++m_TickCountThisSecond;

    RollStatsWindowIfDue();
}

void CFramePacer::OnRenderExecuted()
{
    m_LastRenderTimeSeconds = GetNowSeconds();
    
    ++m_RenderCountThisSecond;

    RollStatsWindowIfDue();
}

void CFramePacer::RollStatsWindowIfDue()
{
    const double Now = GetNowSeconds();

    if (Now - m_StatsWindowStartSeconds < 1.0)
        return;

    m_FrameStats.TicksPerSecond = m_TickCountThisSecond;
    m_FrameStats.FramesPerSecond = m_RenderCountThisSecond;

    m_TickCountThisSecond = 0;
    m_RenderCountThisSecond = 0;
    m_StatsWindowStartSeconds = Now;
}

double CFramePacer::GetNowSeconds()
{
    return static_cast<double>(SDL_GetTicksNS()) / 1'000'000'000.0;
}

void CFramePacer::PlatformSleepSeconds(double Seconds)
{
    #ifdef FUNKIN_PLATFORM_WIN64
        // A waitable timer with high-resolution flag gives sub-millisecond sleep precision on Windows 10 1803+, unlike Sleep()'s ~15.6ms default scheduler granularity.
        static HANDLE TimerHandle = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
            TIMER_ALL_ACCESS);

        if (TimerHandle)
        {
            LARGE_INTEGER DueTime;
            DueTime.QuadPart = -static_cast<int64>(Seconds * 10'000'000.0); // 100ns units, negative = relative

            SetWaitableTimerEx(TimerHandle, &DueTime, 0, nullptr, nullptr, nullptr, 0);
            WaitForSingleObject(TimerHandle, INFINITE);
        }
        else
        {
            // Fallback for pre-1803 systems where CREATE_WAITABLE_TIMER_HIGH_RESOLUTION isn't supported.
            Sleep(static_cast<DWORD>(Seconds * 1000.0));
        }
    #else
        #error "Missing implementation of CFramePacer::PlatformSleepSeconds for the current platform!"
    #endif
}
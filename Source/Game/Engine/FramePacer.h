#pragma once

#include "Core/CoreTypes.h"

struct FFrameStats
{
    uint32 FramesPerSecond = 0;
    uint32 TicksPerSecond = 0;
};

/**
 * Paces two independent, fixed-rate "tick" and "render" gates - off of a single clock, without an accumulator.
 * Neither gate advances the other; each is just "has enough time passed since this gate last fired".
 *
 * This deliberately does NOT know why its intervals are what they are, making it a dumb, reusable two-gate timer.
 * This means the decision on, for example, 240Hz tick rate is a decision that belongs to the owner of an instance of this class, not CFramePacer itself.
 *
 * Usage per loop iteration:
 *   1. Call WaitForNextDeadline() to sleep/spin until the soonest of the two gates is due.
 *   2. Call IsUpdateDue() / IsRenderDue() to check which gate(s) actually opened.
 *   3. Call OnUpdateExecuted() / OnRenderExecuted() after running the corresponding work, to advance that gate's last-fired timestamp and roll its FPS/UPS counter.
 */
class CFramePacer
{
public:
    void Initialize(double TickIntervalSeconds, double RenderIntervalSeconds);

    void SetRenderInterval(double RenderIntervalSeconds) { m_RenderIntervalSeconds = RenderIntervalSeconds; }
    void SetRenderUncapped(bool bUncapped) { bRenderUncapped = bUncapped; }
    bool IsRenderUncapped() const { return bRenderUncapped; }

    // Sleeps for the bulk of the time remaining until the next deadline (whichever gate is sooner), then spin-waits the last SpinMarginSeconds for precision.
    // Does nothing if a gate is already due.
    void WaitForNextDeadline() const;

    bool IsTickDue() const;
    bool IsRenderDue() const;

    // Call after actually performing the update/render work for this iteration.
    // Advances the gate's timestamp and rolls its once-per-second FPS/UPS counter.
    void OnTickExecuted();
    void OnRenderExecuted();

    const FFrameStats& GetFrameStats() const { return m_FrameStats; }

    // Tunable: how long before a deadline WaitForNextDeadline() stops sleeping and starts
    // spinning. Sized to comfortably exceed worst-case OS sleep overshoot.
    void SetSpinMarginSeconds(double SpinMarginSeconds) { m_SpinMarginSeconds = SpinMarginSeconds; }
    double GetSpinMarginSeconds() const { return m_SpinMarginSeconds; }
private:
    double GetNextUpdateDueSeconds() const { return m_LastTickTimeSeconds + m_TickIntervalSeconds; }
    double GetNextRenderDueSeconds() const { return bRenderUncapped ? GetNowSeconds() : m_LastRenderTimeSeconds + m_RenderIntervalSeconds; }

    void RollStatsWindowIfDue();

    static double GetNowSeconds();
    static void PlatformSleepSeconds(double Seconds);
private:
    double m_TickIntervalSeconds = 0.0;
    double m_RenderIntervalSeconds = 0.0;
    bool bRenderUncapped = false;

    double m_LastTickTimeSeconds = 0.0;
    double m_LastRenderTimeSeconds = 0.0;

    double m_SpinMarginSeconds = 0.002; // 2ms

    // Once-per-second FPS/UPS rolling counters, same shape as the classic "reset every 1s" approach.
    uint32 m_TickCountThisSecond = 0;
    uint32 m_RenderCountThisSecond = 0;
    double m_StatsWindowStartSeconds = 0.0;

    FFrameStats m_FrameStats;
};
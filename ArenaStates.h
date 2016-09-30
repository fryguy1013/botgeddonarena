#pragma once

#include "SegmentLedState.h"
#include "ColorTriplet.h"
#include <array>
#include <memory>

class LedAnimation
{
public:
    static constexpr int NumVertical = 48;
    static constexpr int NumHorizontal = 8;
    static constexpr int N = NumVertical*NumHorizontal;

    virtual ~LedAnimation() = default;

    virtual void Tick(bool red, bool blue) = 0;
    virtual std::array<ColorTriplet, N> GetColors() = 0;
    virtual SegmentLedState GetSegmentLed() = 0;
    virtual std::array<bool, 2> GetButtonLeds() = 0;
};

enum class ArenaState {
    Staging,
    PreCountdown,
    Countdown,
    Fighting,
    Paused,
    End
};

class StateMachine
{
public:
    void ChangeState(ArenaState state);

    void Tick(bool red, bool blue) const { _state->Tick(red, blue); }
    std::array<ColorTriplet, LedAnimation::N> GetColors() const { return _state->GetColors(); }
    SegmentLedState GetSegmentLed() const { return _state->GetSegmentLed(); }
    std::array<bool, 2> GetButtonLeds() const { return _state->GetButtonLeds(); }

private:
    std::unique_ptr<LedAnimation> GetAnimation(ArenaState state);

    std::unique_ptr<LedAnimation> _state;
};
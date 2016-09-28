#pragma once

#include "SegmentLedState.h"
#include "ColorTriplet.h"
#include <array>
#include <memory>

class LedAnimation
{
public:
    static const int NumVertical = 48;
    static const int NumHorizontal = 8;
    static const int N = NumVertical*NumHorizontal;

    virtual ~LedAnimation() { }

    virtual void Tick(bool red, bool blue) { }
    virtual std::array<ColorTriplet, N> GetColors() = 0;
    virtual SegmentLedState GetSegmentLed() = 0;
    virtual bool IsAnimationComplete() = 0;
};

enum class ArenaState {
    Staging,
    PreCountdown,
    Countdown,
    Fighting,
    Paused,
    End
};

std::unique_ptr<LedAnimation> GetAnimation(ArenaState state);
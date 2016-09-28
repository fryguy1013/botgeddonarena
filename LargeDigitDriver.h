#pragma once

#include <mraa.hpp>
#include <thread>
#include <mutex>
#include "SegmentLedState.h"

class LargeDigitDriver
{
public:
    LargeDigitDriver(mraa::Gpio& latch, mraa::Gpio& clock, mraa::Gpio& data);
    ~LargeDigitDriver();

    void UpdateSegments(const SegmentLedState& state);
protected:
    void ThreadProc();
    void UpdateLeds(SegmentLedState state);

private:
    std::thread _displayThread;
    std::mutex _displayMutex;
    SegmentLedState _state;
    bool _end;

    mraa::Gpio& _latch;
    mraa::Gpio& _clock;
    mraa::Gpio& _data;
};

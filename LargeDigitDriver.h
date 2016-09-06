#pragma once

#include <mraa.hpp>
#include <thread>
#include <mutex>

struct SegmentLedState
{
    uint8_t segments[4];
    uint8_t dots;
};

class LargeDigitDriver
{
public:
    LargeDigitDriver(mraa::Gpio& latch, mraa::Gpio& clock, mraa::Gpio& data);
    ~LargeDigitDriver();

    void UpdateSegments(const SegmentLedState& state);
protected:
    void ThreadProc();
    uint8_t GetLedSegments(uint8_t number, bool decimal);
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


#include "LargeDigitDriver.h"

#include <chrono>
using namespace std::chrono_literals;



LargeDigitDriver::LargeDigitDriver(mraa::Gpio& latch, mraa::Gpio& clock, mraa::Gpio& data) :
    _end(false), _latch(latch), _clock(clock), _data(data)
{
    //_displayThread = std::thread(std::bind(&LargeDigitDriver::ThreadProc, this));
    _clock.write(0);
    _data.write(0);
    _latch.write(0);
}

LargeDigitDriver::~LargeDigitDriver()
{
    _end = true;
    //_displayThread.join();
}

void LargeDigitDriver::UpdateSegments(const SegmentLedState& state)
{
    //std::lock_guard<std::mutex> lock(_displayMutex);
    _state = state;
    UpdateLeds(state);
}

void LargeDigitDriver::UpdateLeds(SegmentLedState state)
{
    _latch.write(0);

    auto seg = SegmentLedState::GetSegmentDigits(state.segments[2]);
    for (int x = 0; x < 8; x++)
    {
        _clock.write(0);
        _data.write((seg >> (7 - x)) & 0x01);
        std::this_thread::sleep_for(10us);
        _clock.write(1);
    }

    seg = SegmentLedState::GetSegmentDigits(state.segments[1]);
    for (int x = 0; x < 8; x++)
    {
        _clock.write(0);
        _data.write((seg >> (7 - x)) & 0x01);
        std::this_thread::sleep_for(10us);
        _clock.write(1);
    }

    seg = SegmentLedState::GetSegmentDigits(state.segments[0]);
    for (int x = 0; x < 8; x++)
    {
        _clock.write(0);
        _data.write((seg >> (7 - x)) & 0x01);
        std::this_thread::sleep_for(10us);
        _clock.write(1);
    }

    _latch.write(1);
}

void LargeDigitDriver::ThreadProc()
{
    SegmentLedState state;
    SegmentLedState lastState;
    while (!_end)
    {
        {
            std::lock_guard<std::mutex> lock(_displayMutex);
            state = _state;
        }

        if (memcmp(&lastState, &state, sizeof(SegmentLedState)) != 0) {
            UpdateLeds(state);
        }

        //lastState = state;
        std::this_thread::sleep_for(100ms);
    }
}

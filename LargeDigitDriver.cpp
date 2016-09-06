
#include "LargeDigitDriver.h"

#include <chrono>
using namespace std::chrono_literals;

//Given a number, or '-', shifts it out to the display
uint8_t LargeDigitDriver::GetLedSegments(uint8_t number, bool decimal)
{
    //    -  A
    //   / / F/B
    //    -  G
    //   / / E/C
    //    -. D/DP

    const uint8_t a = 1 << 0;
    const uint8_t b = 1 << 6;
    const uint8_t c = 1 << 5;
    const uint8_t d = 1 << 4;
    const uint8_t e = 1 << 3;
    const uint8_t f = 1 << 1;
    const uint8_t g = 1 << 2;
    const uint8_t dp = 1 << 7;

    uint8_t segments = 0;

    switch (number)
    {
    case 1: segments = b | c; break;
    case 2: segments = a | b | d | e | g; break;
    case 3: segments = a | b | c | d | g; break;
    case 4: segments = f | g | b | c; break;
    case 5: segments = a | f | g | c | d; break;
    case 6: segments = a | f | g | e | c | d; break;
    case 7: segments = a | b | c; break;
    case 8: segments = a | b | c | d | e | f | g; break;
    case 9: segments = a | b | c | d | f | g; break;
    case 0: segments = a | b | c | d | e | f; break;
    case '1': segments = b | c; break;
    case '2': segments = a | b | d | e | g; break;
    case '3': segments = a | b | c | d | g; break;
    case '4': segments = f | g | b | c; break;
    case '5': segments = a | f | g | c | d; break;
    case '6': segments = a | f | g | e | c | d; break;
    case '7': segments = a | b | c; break;
    case '8': segments = a | b | c | d | e | f | g; break;
    case '9': segments = a | b | c | d | f | g; break;
    case '0': segments = a | b | c | d | e | f; break;
    case ' ': segments = 0; break;
    case 'c': segments = g | e | d; break;
    case '-': segments = g; break;
    }

    if (decimal) segments |= dp;
    return segments;
}

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

    auto seg = GetLedSegments(state.segments[2], false);
    for (int x = 0; x < 8; x++)
    {
        _clock.write(0);
        _data.write((seg >> (7 - x)) & 0x01);
        std::this_thread::sleep_for(10us);
        _clock.write(1);
    }

    seg = GetLedSegments(state.segments[1], false);
    for (int x = 0; x < 8; x++)
    {
        _clock.write(0);
        _data.write((seg >> (7 - x)) & 0x01);
        std::this_thread::sleep_for(10us);
        _clock.write(1);
    }

    seg = GetLedSegments(state.segments[0], true);
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

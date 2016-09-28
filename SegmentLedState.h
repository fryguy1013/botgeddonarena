#pragma once
#include <cstdint>

struct SegmentLedState
{
    char segments[4];
    uint8_t dots;
    static uint8_t GetSegmentDigits(uint8_t digit);
};

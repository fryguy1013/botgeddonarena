#pragma once

#include "mraa.hpp"
#include <array>
#include <stdint.h>
#include <iostream>

// spec sheet for the WS2812 here:
// http://cdn.sparkfun.com/datasheets/Components/LED/WS2812.pdf

// Information for calculating appropriate timing found here:
// https://cpldcpu.wordpress.com/2014/01/14/light_ws2812-library-v2-0-part-i-understanding-the-ws2812/

struct ColorTriplet
{
    uint8_t r;
    uint8_t g;
    uint8_t b;

    static ColorTriplet FromHsv(uint8_t h, uint8_t s, uint8_t v) {
        unsigned char region, remainder, p, q, t;
        ColorTriplet rgb;

        if (s == 0)
        {
            rgb.r = v;
            rgb.g = v;
            rgb.b = v;
            return rgb;
        }

        region = h / 43;
        remainder = (h - (region * 43)) * 6;

        p = (v * (255 - s)) >> 8;
        q = (v * (255 - ((s * remainder) >> 8))) >> 8;
        t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

        switch (region)
        {
        case 0:
            rgb.r = v; rgb.g = t; rgb.b = p;
            break;
        case 1:
            rgb.r = q; rgb.g = v; rgb.b = p;
            break;
        case 2:
            rgb.r = p; rgb.g = v; rgb.b = t;
            break;
        case 3:
            rgb.r = p; rgb.g = q; rgb.b = v;
            break;
        case 4:
            rgb.r = t; rgb.g = p; rgb.b = v;
            break;
        default:
            rgb.r = v; rgb.g = p; rgb.b = q;
            break;
        }

        return rgb;
    }
};

template <int N>
class LedSegment
{
    static constexpr int SpiBitsPerPixel = 3;
    static constexpr int ColorBitsPerPixel = 24;
    static constexpr int PaddingBits = 16;
    static constexpr int DataLen = (N * ColorBitsPerPixel * SpiBitsPerPixel) / 8 + PaddingBits / 8;

public:
    LedSegment(int channel) : _spi(channel)
    {
        _spi.frequency(2500000);
    }

    ~LedSegment()
    {
        Refresh(std::array<ColorTriplet, N>());
    }

    void Refresh(std::array<ColorTriplet, N> data)
    {
        ConvertNeoToSpi(data);
        _spi.transfer(&_spiBuf[0], nullptr, DataLen);
    }

private:
    void ConvertNeoToSpi(std::array<ColorTriplet, N> data)
    {
        std::fill(_spiBuf.begin(), _spiBuf.end(), 0);
        int bitNum = 0;
        for (auto c : data) {
            uint32_t fullColorBits = ((c.g << 16) | (c.r << 8) | (c.b));
            for (int i = 0; i < 24; i++)
            {
                uint8_t s = ((fullColorBits >> (23 - i)) & 0x01);
                if (s) {
                    _spiBuf[bitNum / 8] |= 1 << (7 - (bitNum % 8));  bitNum++;
                    _spiBuf[bitNum / 8] |= 1 << (7 - (bitNum % 8));  bitNum++;
                    _spiBuf[bitNum / 8] |= 0 << (7 - (bitNum % 8));  bitNum++;
                }
                else {
                    _spiBuf[bitNum / 8] |= 1 << (7 - (bitNum % 8));  bitNum++;
                    _spiBuf[bitNum / 8] |= 0 << (7 - (bitNum % 8));  bitNum++;
                    _spiBuf[bitNum / 8] |= 0 << (7 - (bitNum % 8));  bitNum++;
                }
            }
        }
    }

    mraa::Spi _spi;
    std::array<uint8_t, DataLen> _spiBuf;
};

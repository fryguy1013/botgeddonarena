#pragma once

#include <cstdint>

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

        region = (uint8_t)(h / 43);
        remainder = (uint8_t)((h - (region * 43)) * 6);

        p = (uint8_t)((v * (255 - s)) >> 8);
        q = (uint8_t)((v * (255 - ((s * remainder) >> 8))) >> 8);
        t = (uint8_t)((v * (255 - ((s * (255 - remainder)) >> 8))) >> 8);

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

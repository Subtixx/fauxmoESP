#pragma once

struct RGBColor
{
    RGBColor(uint8_t r, uint8_t g, uint8_t b)
            :red(r), green(g), blue(b) { }

    RGBColor() : red(0), green(0), blue(0) { }

    uint8_t red;
    uint8_t green;
    uint8_t blue;
};
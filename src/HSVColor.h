#pragma once

struct HSVColor
{
    explicit HSVColor(const RGBColor& color)
    {
        float red = ((float)color.red) / 255.0f;
        float green = ((float)color.green) / 255.0f;
        float blue = ((float)color.blue) / 255.0f;

        float mi = std::min(std::min(red, green), blue);
        float ma = std::max(std::max(red, green), blue);
        float diff = ma - mi;

        value = ma;
        hue = 0;
        saturation = 0;
        if (value > 0.0f)
        {
            saturation = (diff / ma);
        }

        if (diff < 0.00001f)
        {
            return;
        }

        if (red == value)
        {
            hue = (green - blue) / diff + (green < blue ? 6.0f : 0.0f);
        }
        else if (green == value)
        {
            hue = (blue - red) / diff + 2.0f;
        }
        else
        {
            hue = (red - green) / diff + 4.0f;
        }
        hue /= 6.0f;
    };

    HSVColor(float hue, float saturation, float value)
            :hue(hue), saturation(saturation), value(value) { };

    float hue;
    float saturation;
    float value;
};
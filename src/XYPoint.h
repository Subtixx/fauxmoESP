#pragma once

struct XYPoint
{
    XYPoint(float x, float y)
            :x(x), y(y) {}

    XYPoint() : XYPoint(0, 0) {}

    float x;
    float y;
};
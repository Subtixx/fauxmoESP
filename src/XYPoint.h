#pragma once

/**
 * @brief Represents a point in 2D space.
 */
struct XYPoint
{
    /**
     * @brief Creates a new XYPoint.
     * @param x The x coordinate.
     * @param y The y coordinate.
     */
    XYPoint(float x, float y)
            :x(x), y(y) {}

    /**
     * @brief Creates a new XYPoint with both coordinates set to 0.
     */
    XYPoint() : XYPoint(0, 0) {}

    /**
     * @brief The x coordinate.
     */
    float x;

    /**
     * @brief The y coordinate.
     */
    float y;
};
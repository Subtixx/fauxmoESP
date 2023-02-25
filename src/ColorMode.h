#pragma once

/**
 * @brief Available color modes
 */
enum ColorMode
{
    /**
     * @brief Hue and Saturation
     */
    COLOR_MODE_HS,

    /**
     * @brief XY
     */
    COLOR_MODE_XY,

    /**
     * @brief Color Temperature
     */
    COLOR_MODE_CT
};

static String ColorModeToString(ColorMode colorMode)
{
    switch (colorMode)
    {
    default:
    case COLOR_MODE_HS:
        return "hs";
    case COLOR_MODE_XY:
        return "xy";
    case COLOR_MODE_CT:
        return "ct";
    }
}

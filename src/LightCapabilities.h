#pragma once

#include <Arduino.h>
#include "XYPoint.h"

#if defined(USE_ARDUINO_JSON)
#include <ArduinoJson.h>
#endif

/**
 * The light capabilities.
 * This is the capabilities of the light, not the current state.
 */
struct LightCapabilities
{
    /**
     * Default constructor.
     * This will set the default values for the light capabilities.
     */
    LightCapabilities();

    /**
     * Constructor.
     * This will set the values for the light capabilities.
     * @param minDimLevel The minimum value that the bri attribute can be set to.
     * @param maxLumen The maximum value that the bri attribute can be set to.
     * @param minColorTemperature The minimum value that the ct attribute can be set to.
     * @param maxColorTemperature The maximum value that the ct attribute can be set to.
     */
    LightCapabilities(uint16_t minDimLevel, uint16_t maxLumen,
            uint16_t minColorTemperature, uint16_t maxColorTemperature);

    /**
     * The minimum value that the bri attribute can be set to.
     * This is the minimum that the light is capable of, note, a brightness of 1 is not off.
     *
     * json key: mindimlevel
     */
    uint16_t minDimLevel;

    /**
     * The maximum value that the bri attribute can be set to.
     * This is the maximum that the light is capable of.
     *
     * json key: maxlumen
     */
    uint16_t maxLumen;

    /**
     * The color gamut type, this is the closest matching gamut type as provided by the Philips Hue documentation.
     * The gamut types are A, B and C.
     * The CIE color space consists of three primary colors (red, green and blue), each having its own x and y coordinate.
     * The color gamut describes the total colors that can be created using these three primary colors.
     * The color gamut is defined by three points, one for each primary color.
     * The color gamut of a light can be found by analyzing the three primary colors of the light.
     * The CIE color space is a cartesian coordinate system, where x represents the amount of green and y represents the amount of red.
     * The color gamut of a light can be found by analyzing the three primary colors of the light.
     * The CIE color space is a cartesian coordinate system, where x represents the amount of green and y represents the amount of red.
     *
     * json key: colorgamuttype
     */
    char colorGamutType;

    /**
     * The color gamut of the light, this is the closest matching gamut as provided by the Philips Hue documentation.
     * The gamut types are A, B and C.
     * The CIE color space consists of three primary colors (red, green and blue), each having its own x and y coordinate.
     * The color gamut describes the total colors that can be created using these three primary colors.
     * The color gamut is defined by three points, one for each primary color.
     * The color gamut of a light can be found by analyzing the three primary colors of the light.
     * The CIE color space is a cartesian coordinate system, where x represents the amount of green and y represents the amount of red.
     * The color gamut of a light can be found by analyzing the three primary colors of the light.
     * The CIE color space is a cartesian coordinate system, where x represents the amount of green and y represents the amount of red.
     *
     * json key: colorgamut
     */
    XYPoint colorGamut[3];

    /**
     * The minimum and maximum mired color temperature the light is capable of.
     *
     * json key: ct.min
     */
    uint16_t minColorTemperature;

    /**
     * The minimum and maximum mired color temperature the light is capable of.
     *
     * json key: ct.max
     */
    uint16_t maxColorTemperature;

    /**
     * Whether the light is "certified" by Philips.
     */
    bool certified = false;

    /**
     * Whether the light supports streaming renderer.
     */
    bool streamingRenderer = true;

    /**
     * Whether the light supports streaming proxy.
     */
    bool streamingProxy = false;

    /**
     * Convert the light capabilities to a JSON string.
     */
#if USE_ARDUINO_JSON
    [[nodiscard]] StaticJsonDocument<384> toJson() const;
#else
    [[nodiscard]] String toJson() const;
#endif
};

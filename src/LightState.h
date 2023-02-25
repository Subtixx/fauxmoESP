#pragma once

#include <Arduino.h>
#include "AlertEffect.h"
#include "LightEffect.h"
#include "XYPoint.h"
#include "ColorMode.h"

#if USE_ARDUINO_JSON
#include <ArduinoJson.h>
#endif

/**
 * This class represents the state of a light.
 */
struct LightState
{
    /**
     * Creates a new light state with default values.
     * @note Default sets isLightReachable to false.
     *       This means no functions will be able to change the state of the light until isLightReachable is set to true.
     */
    LightState();

    /**
     * Creates a new light state with the given values.
     * @param state The on/off state of the light.
     * @param brightness The current brightness of the light.
     * @param hue The current hue of the light.
     * @param saturation The current saturation of the light.
     * @param xy The current xy color of the light.
     * @param colorTemperature The current color temperature of the light.
     * @param alertEffect The current alert effect of the light.
     * @param dynamicLightEffect The current dynamic light effect of the light.
     * @param colorMode The current color mode of the light.
     * @param isLightReachable The current reachability of the light.
     */
    explicit LightState(bool state, uint8_t brightness = 254, uint16_t hue = 41462, uint8_t saturation = 77,
            const XYPoint& xy = XYPoint(),
            uint16_t colorTemperature = 155, AlertEffect alertEffect = ALERT_EFFECT_NONE,
            LightEffect dynamicLightEffect = LIGHT_EFFECT_NONE, ColorMode colorMode = COLOR_MODE_CT,
            bool isLightReachable = true);

    /**
     * On/Off state of the light.
     * On=true, Off=false
     *
     * json key: on
     */
    bool isOn;

    /**
     * Brightness of the light.
     * This is a scale from the minimum brightness the light is capable of,
     * 1, to the maximum capable brightness, 254.
     *
     * json key: bri
     */
    uint8_t brightness;

    /**
     * Hue of the light.
     * This is a wrapping value between 0 and 65535.
     * Note, that hue/sat values are hardware dependent which means that programming two devices
     * with the same value does not garantuee that they will be the same color.
     * Programming 0 and 65535 would mean that the light will resemble the color red,
     * 21845 for green and 43690 for blue.
     *
     * json key: hue
     */
    uint16_t hue;

    /**
     * Saturation of the light.
     * 254 is the most saturated (colored) and 0 is the least saturated (white).
     *
     * json key: sat
     */
    uint8_t saturation;

    /**
     * The x and y coordinates of a color in CIE color space.
     * The first entry is the x coordinate and the second entry is the y coordinate.
     * Both x and y are between 0 and 1. Using CIE xy,
     * the colors can be the same on all lamps if the coordinates are within every lamps gamuts (example: “xy”:[0.409,0.5179] is the same color on all lamps).
     * If not, the lamp will calculate it’s closest color and use that. The CIE xy color is absolute, independent from the hardware.
     *
     * json key: xy
     */
    XYPoint xy;

    /**
     * The Mired Color temperature of the light.
     * 2012 connected lights are capable of 153 (6500K) to 500 (2000K).
     *
     * json key: ct
     */
    uint16_t colorTemperature;

    /**
     * The alert effect, which is a temporary change to the bulb's state. This can take one of the following values:
     *      "none"      – The light is not performing an alert effect.
     *      "select"    – The light is performing one breathe cycle.
     *      "lselect"   – The light is performing breathe cycles for 15 seconds or until an "alert": "none" command is received.
     *
     * Note that this contains the last alert sent to the light and not its current state.
     * i.e. After the breathe cycle has finished the bridge does not reset the alert to “none“.
     *
     * json key: alert
     */
    AlertEffect alertEffect;

    /**
     * The dynamic effect of the light, can either be "none" or "colorloop".
     * If set to colorloop, the light will cycle through all hues using the current brightness and saturation settings.
     *
     * json key: effect
     */
    LightEffect dynamicLightEffect;

    /**
     * Indicates the color mode in which the light is working,
     * this is the last command type it received.
     * Values are
     *      - "hs" for Hue and Saturation
     *      - "xy" for XY
     *      - "ct" for Color Temperature
     * This parameter is only present when the light supports at least one of the values.
     *
     * json key: colormode
     */
    ColorMode colorMode;

    /**
     * Indicates if a light can be reached by the bridge.
     *
     * json key: reachable
     */
    bool isLightReachable;

    String mode = "homeautomation";

    /**
     * Returns the current state of the light as a JSON string.
     */
#if USE_ARDUINO_JSON
    [[nodiscard]] StaticJsonDocument<256> toJson() const;
#else
    [[nodiscard]] String toJson() const;
#endif

    /**
     * Return the current color mode as a string.
     * @return The current color mode as a string.
     */
    [[nodiscard]] String getColorModeString() const;

    /**
     * Return the current dynamic light effect as a string.
     * @return The current dynamic light effect as a string.
     */
    [[nodiscard]] String getDynamicLightEffectString() const;

    /**
     * Return the current alert effect as a string.
     * @return The current alert effect as a string.
     */
    [[nodiscard]] String getAlertEffectString() const;

    bool operator==(const LightState& other) const;
    bool operator!=(const LightState& other) const;
};

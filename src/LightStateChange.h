#pragma once
/*
on 	bool 	On/Off state of the light. On=true, Off=false 	Optional
bri 	uint8 	The brightness value to set the light to.Brightness is a scale from 1 (the minimum the light is capable of) to 254 (the maximum).

Note: a brightness of 1 is not off.

e.g. “brightness”: 60 will set the light to a specific brightness
	Optional
hue 	uint16 	The hue value to set light to.The hue value is a wrapping value between 0 and 65535. Both 0 and 65535 are red, 25500 is green and 46920 is blue.

e.g. “hue”: 50000 will set the light to a specific hue.
	Optional
sat 	uint8 	Saturation of the light. 254 is the most saturated (colored) and 0 is the least saturated (white). 	Optional
xy 	list 2..2 of float 4 	The x and y coordinates of a color in CIE color space.The first entry is the x coordinate and the second entry is the y coordinate. Both x and y must be between 0 and 1.
If the specified coordinates are not in the CIE color space, the closest color to the coordinates will be chosen. 	Optional
ct 	uint16 	The Mired color temperature of the light. 2012 connected lights are capable of 153 (6500K) to 500 (2000K). 	Optional
alert 	string 	The alert effect,is a temporary change to the bulb’s state, and has one of the following values:
“none” – The light is not performing an alert effect.
“select” – The light is performing one breathe cycle.
“lselect” – The light is performing breathe cycles for 15 seconds or until an "alert": "none" command is received.Note that this contains the last alert sent to the light and not its current state. i.e. After the breathe cycle has finished the bridge does not reset the alert to “none“. 	Optional
effect 	string 	The dynamic effect of the light. Currently “none” and “colorloop” are supported. Other values will generate an error of type 7.Setting the effect to colorloop will cycle through all hues using the current brightness and saturation settings. 	Optional
transitiontime 	uint16 	The duration of the transition from the light’s current state to the new state. This is given as a multiple of 100ms and defaults to 4 (400ms). For example, setting transitiontime:10 will make the transition last 1 second. 	Optional
bri_inc 	-254 to 254 	Increments or decrements the value of the brightness.  bri_inc is ignored if the bri attribute is provided. Any ongoing bri transition is stopped. Setting a value of 0 also stops any ongoing transition. The bridge will return the bri value after the increment is performed. 	 Optional
sat_inc 	-254 to 254 	Increments or decrements the value of the sat.  sat_inc is ignored if the sat attribute is provided. Any ongoing sat transition is stopped. Setting a value of 0 also stops any ongoing transition. The bridge will return the sat value after the increment is performed. 	Optional
hue_inc 	-65534 to 65534 	Increments or decrements the value of the hue.   hue_inc is ignored if the hue attribute is provided. Any ongoing color transition is stopped. Setting a value of 0 also stops any ongoing transition. The bridge will return the hue value after the increment is performed.Note if the resulting values are < 0 or > 65535 the result is wrapped. For example:

{"hue_inc":  1}

on a hue value of 65535 results in a hue of 0.
{"hue_inc":  -2}

on a hue value of 0 results in a hue of 65534.
	Optional
 ct_inc 	 -65534 to 65534 	Increments or decrements the value of the ct. ct_inc is ignored if the ct attribute is provided. Any ongoing color transition is stopped. Setting a value of 0 also stops any ongoing transition. The bridge will return the ct value after the increment is performed. 	 Optional
 xy_inc 	 list 2..2 of float 4 	Increments or decrements the value of the xy.  xy_inc is ignored if the xy attribute is provided. Any ongoing color transition is stopped. Setting a value of 0 also stops any ongoing transition. Will stop at it’s gamut boundaries. The bridge will return the xy value after the increment is performed. Max value [0.5, 0.5]. 	Optional
 */

#include <Arduino.h>
#include "AlertEffect.h"
#include "LightEffect.h"
#include "XYPoint.h"

struct LightStateChange
{
public:
    explicit LightStateChange(const String& jsonString);
    String getChanges(int lightId);
private:
    /**
     * json key: on
     */
    bool _isOn = false;
    bool _isOnSet = false;
    bool _isOnSuccessfullySet = false;

    /**
     * json key: bri
     */
    uint8_t _brightness = 0;
    bool _brightnessSet = false;
    bool _brightnessSuccessfullySet = false;

    /**
     * json key: hue
     */
    uint16_t _hue = 0;
    bool _hueSet = false;
    bool _hueSuccessfullySet = false;

    /**
     * json key: sat
     */
    uint8_t _saturation = 0;
    bool _saturationSet = false;
    bool _saturationSuccessfullySet = false;

    /**
     * json key: xy
     */
    XYPoint _xyPoint = XYPoint(0, 0);
    bool _xyPointSet = false;
    bool _xyPointSuccessfullySet = false;

    /**
     * json key: ct
     */
    uint16_t _colorTemperature = 0;
    bool _colorTemperatureSet = false;
    bool _colorTemperatureSuccessfullySet = false;

    /**
     * json key: alert
     */
    AlertEffect _alertEffect = AlertEffect::ALERT_EFFECT_NONE;
    bool _alertEffectSet = false;
    bool _alertEffectSuccessfullySet = false;

    /**
     * json key: effect
     */
    LightEffect _lightEffect = LightEffect::LIGHT_EFFECT_NONE;
    bool _lightEffectSet = false;
    bool _lightEffectSuccessfullySet = false;

    /**
     * json key: transitiontime
     */
    uint16_t _transitionTime = 0;
    bool _transitionTimeSet = false;
    bool _transitionTimeSuccessfullySet = false;

    /**
     * json key: bri_inc
     */
    int16_t _brightnessIncrement = 0;
    bool _brightnessIncrementSet = false;
    bool _brightnessIncrementSuccessfullySet = false;

    /**
     * json key: sat_inc
     */
    int16_t _saturationIncrement = 0;
    bool _saturationIncrementSet = false;
    bool _saturationIncrementSuccessfullySet = false;

    /**
     * json key: hue_inc
     */
    int32_t _hueIncrement = 0;
    bool _hueIncrementSet = false;
    bool _hueIncrementSuccessfullySet = false;

    /**
     * json key: ct_inc
     */
    int32_t _colorTemperatureIncrement = 0;
    bool _colorTemperatureIncrementSet = false;
    bool _colorTemperatureIncrementSuccessfullySet = false;

    /**
     * json key: xy_inc
     */
    XYPoint _xyPointIncrement = XYPoint(0, 0);
    bool _xyPointIncrementSet = false;
    bool _xyPointIncrementSuccessfullySet = false;

public:
    [[nodiscard]] bool getIsOn() const;
    [[nodiscard]] bool isOnSet() const;
    [[nodiscard]] bool isOnSuccessfullySet() const;
    void setOnSuccess(bool success);

    [[nodiscard]] uint8_t getBrightness() const;
    [[nodiscard]] bool isBrightnessSet() const;
    [[nodiscard]] bool isBrightnessSuccessfullySet() const;
    void setBrightnessSuccess(bool success);

    [[nodiscard]] uint16_t getHue() const;
    [[nodiscard]] bool isHueSet() const;
    [[nodiscard]] bool isHueSuccessfullySet() const;
    void setHueSuccess(bool success);

    [[nodiscard]] uint8_t getSaturation() const;
    [[nodiscard]] bool isSaturationSet() const;
    [[nodiscard]] bool isSaturationSuccessfullySet() const;
    void setSaturationSuccess(bool success);

    [[nodiscard]] const XYPoint& getXyPoint() const;
    [[nodiscard]] bool isXyPointSet() const;
    [[nodiscard]] bool isXyPointSuccessfullySet() const;
    void setXyPointSuccess(bool success);

    [[nodiscard]] uint16_t getColorTemperature() const;
    [[nodiscard]] bool isColorTemperatureSet() const;
    [[nodiscard]] bool isColorTemperatureSuccessfullySet() const;
    void setColorTemperatureSuccess(bool success);

    [[nodiscard]] AlertEffect getAlertEffect() const;
    [[nodiscard]] bool isAlertEffectSet() const;
    [[nodiscard]] bool isAlertEffectSuccessfullySet() const;
    void setAlertEffectSuccess(bool success);

    [[nodiscard]] LightEffect getLightEffect() const;
    [[nodiscard]] bool isLightEffectSet() const;
    [[nodiscard]] bool isLightEffectSuccessfullySet() const;
    void setLightEffectSuccess(bool success);

    [[nodiscard]] uint16_t getTransitionTime() const;
    [[nodiscard]] bool isTransitionTimeSet() const;
    [[nodiscard]] bool isTransitionTimeSuccessfullySet() const;
    void setTransitionTimeSuccess(bool success);

    [[nodiscard]] int16_t getBrightnessIncrement() const;
    [[nodiscard]] bool isBrightnessIncrementSet() const;
    [[nodiscard]] bool isBrightnessIncrementSuccessfullySet() const;
    void setBrightnessIncrementSuccess(bool success);

    [[nodiscard]] int16_t getSaturationIncrement() const;
    [[nodiscard]] bool isSaturationIncrementSet() const;
    [[nodiscard]] bool isSaturationIncrementSuccessfullySet() const;
    void setSaturationIncrementSuccess(bool success);

    [[nodiscard]] int32_t getHueIncrement() const;
    [[nodiscard]] bool isHueIncrementSet() const;
    [[nodiscard]] bool isHueIncrementSuccessfullySet() const;
    void setHueIncrementSuccess(bool success);

    [[nodiscard]] int32_t getColorTemperatureIncrement() const;
    [[nodiscard]] bool isColorTemperatureIncrementSet() const;
    [[nodiscard]] bool isColorTemperatureIncrementSuccessfullySet() const;
    void setColorTemperatureIncrementSuccess(bool success);

    [[nodiscard]] const XYPoint& getXyPointIncrement() const;
    [[nodiscard]] bool isXyPointIncrementSet() const;
    [[nodiscard]] bool isXyPointIncrementSuccessfullySet() const;
    void setXyPointIncrementSuccess(bool success);
};
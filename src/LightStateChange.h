#pragma once
#include <Arduino.h>
#include "AlertEffect.h"
#include "LightEffect.h"
#include "XYPoint.h"

/**
 * @brief Represents a state change of a light device.
 */
struct LightStateChange
{
public:
    /**
     * @brief Creates a new light state change from a json string.
     */
    explicit LightStateChange(const String& jsonString);

    /**
     * @brief Returns all changes as a json string.
     */
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
    /**
     * @brief Returns whether the light should be turned on or off.
     * @return true if the light should be turned on, false if the light should be turned off.
     */
    [[nodiscard]] bool getIsOn() const;
    /**
     * @brief Returns whether the on state was set.
     * @return true if the on state was set, false otherwise.
     */
    [[nodiscard]] bool isOnSet() const;
    /**
     * @brief Returns whether the on state was successfully set.
     * @return true if the on state was successfully set, false otherwise.
     */
    [[nodiscard]] bool isOnSuccessfullySet() const;
    /**
     * @brief Sets whether the on state was successfully set.
     * @param success true if the on state was successfully set, false otherwise.
     */
    void setOnSuccess(bool success);

    /**
     * @brief Returns the new brightness of the light.
     * @return the brightness of the light.
     */
    [[nodiscard]] uint8_t getBrightness() const;
    /**
     * @brief Returns whether the brightness was set.
     * @return true if the brightness was set, false otherwise.
     */
    [[nodiscard]] bool isBrightnessSet() const;
    /**
     * @brief Returns whether the brightness was successfully set.
     * @return true if the brightness was successfully set, false otherwise.
     */
    [[nodiscard]] bool isBrightnessSuccessfullySet() const;
    /**
     * @brief Sets whether the brightness was successfully set.
     * @param success true if the brightness was successfully set, false otherwise.
     */
    void setBrightnessSuccess(bool success);

    /**
     * @brief Returns the new hue of the light.
     * @return the hue of the light.
     */
    [[nodiscard]] uint16_t getHue() const;
    /**
     * @brief Returns whether the hue was set.
     * @return true if the hue was set, false otherwise.
     */
    [[nodiscard]] bool isHueSet() const;
    /**
     * @brief Returns whether the hue was successfully set.
     * @return true if the hue was successfully set, false otherwise.
     */
    [[nodiscard]] bool isHueSuccessfullySet() const;
    /**
     * @brief Sets whether the hue was successfully set.
     * @param success true if the hue was successfully set, false otherwise.
     */
    void setHueSuccess(bool success);

    /**
     * @brief Returns the new saturation of the light.
     * @return the saturation of the light.
     */
    [[nodiscard]] uint8_t getSaturation() const;
    /**
     * @brief Returns whether the saturation was set.
     * @return true if the saturation was set, false otherwise.
     */
    [[nodiscard]] bool isSaturationSet() const;
    /**
     * @brief Returns whether the saturation was successfully set.
     * @return true if the saturation was successfully set, false otherwise.
     */
    [[nodiscard]] bool isSaturationSuccessfullySet() const;
    /**
     * @brief Sets whether the saturation was successfully set.
     * @param success true if the saturation was successfully set, false otherwise.
     */
    void setSaturationSuccess(bool success);

    /**
     * @brief Returns the new xy point of the light.
     * @return the xy point of the light.
     */
    [[nodiscard]] const XYPoint& getXyPoint() const;
    /**
     * @brief Returns whether the xy point was set.
     * @return true if the xy point was set, false otherwise.
     */
    [[nodiscard]] bool isXyPointSet() const;
    /**
     * @brief Returns whether the xy point was successfully set.
     * @return true if the xy point was successfully set, false otherwise.
     */
    [[nodiscard]] bool isXyPointSuccessfullySet() const;
    /**
     * @brief Sets whether the xy point was successfully set.
     * @param success true if the xy point was successfully set, false otherwise.
     */
    void setXyPointSuccess(bool success);

    /**
     * @brief Returns the new color temperature of the light.
     * @return the color temperature of the light.
     */
    [[nodiscard]] uint16_t getColorTemperature() const;
    /**
     * @brief Returns whether the color temperature was set.
     * @return true if the color temperature was set, false otherwise.
     */
    [[nodiscard]] bool isColorTemperatureSet() const;
    /**
     * @brief Returns whether the color temperature was successfully set.
     * @return true if the color temperature was successfully set, false otherwise.
     */
    [[nodiscard]] bool isColorTemperatureSuccessfullySet() const;
    /**
     * @brief Sets whether the color temperature was successfully set.
     * @param success true if the color temperature was successfully set, false otherwise.
     */
    void setColorTemperatureSuccess(bool success);

    /**
     * @brief Returns the new alert effect of the light.
     * @return the alert effect of the light.
     */
    [[nodiscard]] AlertEffect getAlertEffect() const;
    /**
     * @brief Returns whether the alert effect was set.
     * @return true if the alert effect was set, false otherwise.
     */
    [[nodiscard]] bool isAlertEffectSet() const;
    /**
     * @brief Returns whether the alert effect was successfully set.
     * @return true if the alert effect was successfully set, false otherwise.
     */
    [[nodiscard]] bool isAlertEffectSuccessfullySet() const;
    /**
     * @brief Sets whether the alert effect was successfully set.
     * @param success true if the alert effect was successfully set, false otherwise.
     */
    void setAlertEffectSuccess(bool success);

    /**
     * @brief Returns the new effect of the light.
     * @return the effect of the light.
     */
    [[nodiscard]] LightEffect getLightEffect() const;
    /**
     * @brief Returns whether the effect was set.
     * @return true if the effect was set, false otherwise.
     */
    [[nodiscard]] bool isLightEffectSet() const;
    /**
     * @brief Returns whether the effect was successfully set.
     * @return true if the effect was successfully set, false otherwise.
     */
    [[nodiscard]] bool isLightEffectSuccessfullySet() const;
    /**
     * @brief Sets whether the effect was successfully set.
     * @param success true if the effect was successfully set, false otherwise.
     */
    void setLightEffectSuccess(bool success);

    /**
     * @brief Returns the new transition time of the light.
     * @return the transition time of the light.
     */
    [[nodiscard]] uint16_t getTransitionTime() const;
    /**
     * @brief Returns whether the transition time was set.
     * @return true if the transition time was set, false otherwise.
     */
    [[nodiscard]] bool isTransitionTimeSet() const;
    /**
     * @brief Returns whether the transition time was successfully set.
     * @return true if the transition time was successfully set, false otherwise.
     */
    [[nodiscard]] bool isTransitionTimeSuccessfullySet() const;
    /**
     * @brief Sets whether the transition time was successfully set.
     * @param success true if the transition time was successfully set, false otherwise.
     */
    void setTransitionTimeSuccess(bool success);

    /**
     * @brief Returns the new brightness increment of the light.
     * @return the brightness increment of the light.
     */
    [[nodiscard]] int16_t getBrightnessIncrement() const;
    /**
     * @brief Returns whether the brightness increment was set.
     * @return true if the brightness increment was set, false otherwise.
     */
    [[nodiscard]] bool isBrightnessIncrementSet() const;
    /**
     * @brief Returns whether the brightness increment was successfully set.
     * @return true if the brightness increment was successfully set, false otherwise.
     */
    [[nodiscard]] bool isBrightnessIncrementSuccessfullySet() const;
    /**
     * @brief Sets whether the brightness increment was successfully set.
     * @param success true if the brightness increment was successfully set, false otherwise.
     */
    void setBrightnessIncrementSuccess(bool success);

    /**
     * @brief Returns the new saturation increment of the light.
     * @return the saturation increment of the light.
     */
    [[nodiscard]] int16_t getSaturationIncrement() const;
    /**
     * @brief Returns whether the saturation increment was set.
     * @return true if the saturation increment was set, false otherwise.
     */
    [[nodiscard]] bool isSaturationIncrementSet() const;
    /**
     * @brief Returns whether the saturation increment was successfully set.
     * @return true if the saturation increment was successfully set, false otherwise.
     */
    [[nodiscard]] bool isSaturationIncrementSuccessfullySet() const;
    /**
     * @brief Sets whether the saturation increment was successfully set.
     * @param success true if the saturation increment was successfully set, false otherwise.
     */
    void setSaturationIncrementSuccess(bool success);

    /**
     * @brief Returns the new hue increment of the light.
     * @return the hue increment of the light.
     */
    [[nodiscard]] int32_t getHueIncrement() const;
    /**
     * @brief Returns whether the hue increment was set.
     * @return true if the hue increment was set, false otherwise.
     */
    [[nodiscard]] bool isHueIncrementSet() const;
    /**
     * @brief Returns whether the hue increment was successfully set.
     * @return true if the hue increment was successfully set, false otherwise.
     */
    [[nodiscard]] bool isHueIncrementSuccessfullySet() const;
    /**
     * @brief Sets whether the hue increment was successfully set.
     * @param success true if the hue increment was successfully set, false otherwise.
     */
    void setHueIncrementSuccess(bool success);

    /**
     * @brief Returns the new color temperature increment of the light.
     * @return the color temperature increment of the light.
     */
    [[nodiscard]] int32_t getColorTemperatureIncrement() const;
    /**
     * @brief Returns whether the color temperature increment was set.
     * @return true if the color temperature increment was set, false otherwise.
     */
    [[nodiscard]] bool isColorTemperatureIncrementSet() const;
    /**
     * @brief Returns whether the color temperature increment was successfully set.
     * @return true if the color temperature increment was successfully set, false otherwise.
     */
    [[nodiscard]] bool isColorTemperatureIncrementSuccessfullySet() const;
    /**
     * @brief Sets whether the color temperature increment was successfully set.
     * @param success true if the color temperature increment was successfully set, false otherwise.
     */
    void setColorTemperatureIncrementSuccess(bool success);

    /**
     * @brief Returns the new xy point of the light.
     * @return the xy point of the light.
     */
    [[nodiscard]] const XYPoint& getXyPointIncrement() const;
    /**
     * @brief Returns whether the xy point was set.
     * @return true if the xy point was set, false otherwise.
     */
    [[nodiscard]] bool isXyPointIncrementSet() const;
    /**
     * @brief Returns whether the xy point was successfully set.
     * @return true if the xy point was successfully set, false otherwise.
     */
    [[nodiscard]] bool isXyPointIncrementSuccessfullySet() const;
    /**
     * @brief Sets whether the xy point was successfully set.
     * @param success true if the xy point was successfully set, false otherwise.
     */
    void setXyPointIncrementSuccess(bool success);
};
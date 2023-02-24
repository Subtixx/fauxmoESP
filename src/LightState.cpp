#include "LightState.h"
#include "AlertEffect.h"
#include "LightEffect.h"
#include "XYPoint.h"
#include "Definitions.h"
#if USE_ARDUINO_JSON
#include <ArduinoJson.h>
#endif

LightState::LightState() :LightState(false)
{
    isLightReachable = false;
}

LightState::LightState(bool state, uint8_t brightness, uint16_t hue, uint8_t saturation, const XYPoint& xy,
        uint16_t colorTemperature, AlertEffect alertEffect, LightEffect dynamicLightEffect, ColorMode colorMode,
        bool isLightReachable)
        :isOn(state), brightness(brightness), hue(hue), saturation(saturation), xy(xy),
         colorTemperature(colorTemperature), alertEffect(alertEffect), dynamicLightEffect(dynamicLightEffect),
         colorMode(colorMode), isLightReachable(isLightReachable)
{

}

#if USE_ARDUINO_JSON
StaticJsonDocument<256> LightState::toJson() const
#else
String LightState::toJson() const
#endif
{
    String colorModeStr = getColorModeString();
    String alertEffectStr = getAlertEffectString();
    String dynamicLightEffectStr = getDynamicLightEffectString();

    #if USE_ARDUINO_JSON
    StaticJsonDocument<256> doc;
    doc["on"] = isOn;
    doc["bri"] = brightness;
    doc["hue"] = hue;
    doc["sat"] = saturation;
    doc["xy"][0] = xy.x;
    doc["xy"][1] = xy.y;
    doc["ct"] = colorTemperature;
    doc["alert"] = alertEffectStr;
    doc["effect"] = dynamicLightEffectStr;
    doc["colormode"] = colorModeStr;
    doc["reachable"] = isLightReachable;
    doc["mode"] = mode;
    
    return doc;
    #else
    char buffer[256];
    sprintf(buffer,
            R"({"on":%s,"bri":%d,"hue":%d,"sat":%d,"xy":[%f,%f],"ct":%d,"alert":"%s","effect":"%s","colormode":"%s","reachable":%s,"mode":"%s"})",
            isOn ? "true" : "false", brightness, hue, saturation, xy.x, xy.y, colorTemperature, alertEffectStr.c_str(),
            dynamicLightEffectStr.c_str(), colorModeStr.c_str(), isLightReachable ? "true" : "false", mode.c_str());

    return { buffer };
    #endif
}

String LightState::getColorModeString() const
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

String LightState::getDynamicLightEffectString() const
{
    switch (dynamicLightEffect)
    {
    case LIGHT_EFFECT_NONE:
        return "none";
    case LIGHT_EFFECT_COLORLOOP:
        return "colorloop";
    default:
        return "none";
    }
}

String LightState::getAlertEffectString() const
{
    switch (alertEffect)
    {
    case ALERT_EFFECT_NONE:
        return "none";
    case ALERT_EFFECT_SELECT:
        return "select";
    case ALERT_EFFECT_LSELECT:
        return "lselect";
    default:
        return "none";
    }
}

bool LightState::operator==(const LightState& other) const
{
    return brightness == other.brightness && hue == other.hue && saturation == other.saturation &&
            xy.x == other.xy.x && xy.y == other.xy.y && colorTemperature == other.colorTemperature
            && alertEffect == other.alertEffect &&
            dynamicLightEffect == other.dynamicLightEffect && colorMode == other.colorMode;
}

bool LightState::operator!=(const LightState& other) const
{
    return !(*this == other);
}

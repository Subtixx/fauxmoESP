#include "LightState.h"
#include "AlertEffect.h"
#include "LightEffect.h"
#include "XYPoint.h"

LightState::LightState()
        :LightState(false, 0, 0, 0, XYPoint(), 0, AlertEffect::ALERT_EFFECT_NONE, LightEffect::LIGHT_EFFECT_NONE,
        COLOR_MODE_HS, false)
{
}

LightState::LightState(bool state, uint8_t brightness, uint16_t hue, uint8_t saturation, const XYPoint& xy,
        uint16_t colorTemperature, AlertEffect alertEffect, LightEffect dynamicLightEffect, ColorMode colorMode,
        bool isLightReachable)
        :state(state), brightness(brightness), hue(hue), saturation(saturation), xy(xy),
         colorTemperature(colorTemperature), alertEffect(alertEffect), dynamicLightEffect(dynamicLightEffect),
         colorMode(colorMode), isLightReachable(isLightReachable)
{

}

String LightState::toJson() const {
    String colorModeStr = getColorModeString();
    String alertEffectStr = getAlertEffectString();
    String dynamicLightEffectStr = getDynamicLightEffectString();

    #if USE_ARDUINO_JSON
    StaticJsonDocument<256> doc;
        doc["on"] = state;
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

        String json;
        serializeJson(doc, json);
        return json;
    #else
    return String(R"({"on":)" + String(state) + R"(,"bri":)" + String(brightness) + R"(,"hue":)" + String(hue)
            + R"(,"sat":)" + String(saturation) + R"(,"xy":[)" + String(xy.x) + R"(,)" + String(xy.y) + R"(],"ct":)"
            + String(colorTemperature) + R"(,"alert":")" + String(alertEffectStr) + R"(","effect":")"
            + String(dynamicLightEffectStr) + R"(","colormode":")" + colorModeStr + R"(","reachable":)"
            + String(isLightReachable) + R"(})");
    #endif
}

String LightState::getColorModeString() const {
    String colorModeStr;
    switch (colorMode)
    {
    default:
    case COLOR_MODE_HS:
        colorModeStr = "hs";
        break;
    case COLOR_MODE_XY:
        colorModeStr = "xy";
        break;
    case COLOR_MODE_CT:
        colorModeStr = "ct";
        break;
    }
    return colorModeStr;
}

String LightState::getDynamicLightEffectString() const {
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

String LightState::getAlertEffectString() const {
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
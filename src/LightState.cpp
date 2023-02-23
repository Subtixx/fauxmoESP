#include "LightState.h"
#include "AlertEffect.h"
#include "LightEffect.h"
#include "XYPoint.h"
#if USE_ARDUINO_JSON
#include "Definitions.h"
#include <ArduinoJson.h>
#endif

LightState::LightState()
        :LightState(false, 0, 0, 0, XYPoint(), 0, AlertEffect::ALERT_EFFECT_NONE, LightEffect::LIGHT_EFFECT_NONE,
        COLOR_MODE_HS, false)
{
}

LightState::LightState(bool state, uint8_t brightness, uint16_t hue, uint8_t saturation, const XYPoint& xy,
        uint16_t colorTemperature, AlertEffect alertEffect, LightEffect dynamicLightEffect, ColorMode colorMode,
        bool isLightReachable)
        :isOn(state), brightness(brightness), hue(hue), saturation(saturation), xy(xy),
         colorTemperature(colorTemperature), alertEffect(alertEffect), dynamicLightEffect(dynamicLightEffect),
         colorMode(colorMode), isLightReachable(isLightReachable)
{

}

String LightState::toJson() const
{
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
    return String(R"({"on":)" + String(isOn) + R"(,"bri":)" + String(brightness) + R"(,"hue":)" + String(hue)
            + R"(,"sat":)" + String(saturation) + R"(,"xy":[)" + String(xy.x) + R"(,)" + String(xy.y) + R"(],"ct":)"
            + String(colorTemperature) + R"(,"alert":")" + String(alertEffectStr) + R"(","effect":")"
            + String(dynamicLightEffectStr) + R"(","colormode":")" + colorModeStr + R"(","reachable":)"
            + String(isLightReachable) + R"(})");
    #endif
}

String LightState::getColorModeString() const
{
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

LightState LightState::fromJson(const String& json)
{
    #if USE_ARDUINO_JSON
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Error parsing JSON: %s\n", error.c_str())
        return {};
    }

    bool state = doc["on"];
    uint8_t brightness = doc["bri"];
    uint16_t hue = doc["hue"];
    uint8_t saturation = doc["sat"];
    uint16_t colorTemperature = doc["ct"];
    AlertEffect alertEffect = AlertEffect::ALERT_EFFECT_NONE;
    LightEffect dynamicLightEffect = LightEffect::LIGHT_EFFECT_NONE;
    ColorMode colorMode = ColorMode::COLOR_MODE_HS;
    XYPoint xy = XYPoint(doc["xy"][0], doc["xy"][1]);

    return { state, brightness, hue, saturation, xy, colorTemperature, alertEffect, dynamicLightEffect, colorMode,
             true };
    #else
    String jsonStr = json;
    jsonStr.replace(" ", "");
    jsonStr.replace("\n", "");

    bool state = false;
    uint8_t brightness = 0;
    uint16_t hue = 0;
    uint8_t saturation = 0;
    uint16_t colorTemperature = 0;
    AlertEffect alertEffect = AlertEffect::ALERT_EFFECT_NONE;
    LightEffect dynamicLightEffect = LightEffect::LIGHT_EFFECT_NONE;
    ColorMode colorMode = ColorMode::COLOR_MODE_HS;
    XYPoint xy = XYPoint(0, 0);

    int onIndex = json.indexOf(R"("on":)");
    if (onIndex != -1)
    {
        state = json.substring(onIndex + 5, json.indexOf(',', onIndex)).toInt();
    }

    int briIndex = json.indexOf(R"("bri":)");
    if (briIndex != -1)
    {
        brightness = json.substring(briIndex + 6, json.indexOf(',', briIndex)).toInt();
    }

    int hueIndex = json.indexOf(R"("hue":)");
    if (hueIndex != -1)
    {
        hue = json.substring(hueIndex + 6, json.indexOf(',', hueIndex)).toInt();
    }

    int satIndex = json.indexOf(R"("sat":)");
    if (satIndex != -1)
    {
        saturation = json.substring(satIndex + 6, json.indexOf(',', satIndex)).toInt();
    }

    int ctIndex = json.indexOf(R"("ct":)");
    if (ctIndex != -1)
    {
        colorTemperature = json.substring(ctIndex + 5, json.indexOf(',', ctIndex)).toInt();
    }

    int xyIndex = json.indexOf(R"("xy":[)");
    if (xyIndex != -1)
    {
        int xyEndIndex = json.indexOf(']', xyIndex);
        int xyCommaIndex = json.indexOf(',', xyIndex);
        xy.x = json.substring(xyIndex + 6, xyCommaIndex).toFloat();
        xy.y = json.substring(xyCommaIndex + 1, xyEndIndex).toFloat();
    }

    int alertIndex = json.indexOf(R"("alert":")");
    if (alertIndex != -1)
    {
        int alertEndIndex = json.indexOf('"', alertIndex + 9);
        String alertStr = json.substring(alertIndex + 9, alertEndIndex);
        if (alertStr == "none")
        {
            alertEffect = AlertEffect::ALERT_EFFECT_NONE;
        }
        else if (alertStr == "select")
        {
            alertEffect = AlertEffect::ALERT_EFFECT_SELECT;
        }
        else if (alertStr == "lselect")
        {
            alertEffect = AlertEffect::ALERT_EFFECT_LSELECT;
        }
    }

    int effectIndex = json.indexOf(R"("effect":")");
    if (effectIndex != -1)
    {
        int effectEndIndex = json.indexOf('"', effectIndex + 10);
        String effectStr = json.substring(effectIndex + 10, effectEndIndex);
        if (effectStr == "none")
        {
            dynamicLightEffect = LightEffect::LIGHT_EFFECT_NONE;
        }
        else if (effectStr == "colorloop")
        {
            dynamicLightEffect = LightEffect::LIGHT_EFFECT_COLORLOOP;
        }
    }

    int colormodeIndex = json.indexOf(R"("colormode":")");
    if (colormodeIndex != -1)
    {
        int colormodeEndIndex = json.indexOf('"', colormodeIndex + 13);
        String colormodeStr = json.substring(colormodeIndex + 13, colormodeEndIndex);
        if (colormodeStr == "hs")
        {
            colorMode = ColorMode::COLOR_MODE_HS;
        }
        else if (colormodeStr == "xy")
        {
            colorMode = ColorMode::COLOR_MODE_XY;
        }
        else if (colormodeStr == "ct")
        {
            colorMode = ColorMode::COLOR_MODE_CT;
        }
    }

    return { state, brightness, hue, saturation, xy, colorTemperature, alertEffect, dynamicLightEffect, colorMode,
             true };
    #endif
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

bool LightState::isValidChange(const LightState& oldState) const
{
    if (isOn != oldState.isOn)
    {
        return true;
    }

    if (!oldState.isOn){
        return this == &oldState;
    }

    return true;
}

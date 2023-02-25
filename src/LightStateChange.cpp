#include <Arduino.h>
#if defined(USE_ARDUINO_JSON)
#include <ArduinoJson.h>
#endif

#include <vector>

#include "LightStateChange.h"
#include "Definitions.h"

LightStateChange::LightStateChange(const String& jsonString)
{
    #if defined(USE_ARDUINO_JSON)
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, jsonString);
    if (error)
    {
        DEBUG_MSG_FAUXMO("[ERROR] deserializeJson() failed: %s\n", error.c_str())
        return;
    }

    if (doc.containsKey("on"))
    {
        _isOnSet = true;
        _isOn = doc["on"];
    }

    if (doc.containsKey("bri"))
    {
        _brightnessSet = true;
        _brightness = doc["bri"];
    }

    if (doc.containsKey("hue"))
    {
        _hueSet = true;
        _hue = doc["hue"];
    }

    if (doc.containsKey("sat"))
    {
        _saturationSet = true;
        _saturation = doc["sat"];
    }

    if (doc.containsKey("xy"))
    {
        _xyPointSet = true;
        _xyPoint = XYPoint(doc["xy"][0], doc["xy"][1]);
    }

    if (doc.containsKey("ct"))
    {
        _colorTemperatureSet = true;
        _colorTemperature = doc["ct"];
    }

    if (doc.containsKey("alert"))
    {
        _alertEffectSet = true;
        String alertEffect = doc["alert"];
        if (alertEffect == "none")
        {
            _alertEffect = AlertEffect::ALERT_EFFECT_NONE;
        }
        else if (alertEffect == "select")
        {
            _alertEffect = AlertEffect::ALERT_EFFECT_SELECT;
        }
        else if (alertEffect == "lselect")
        {
            _alertEffect = AlertEffect::ALERT_EFFECT_LSELECT;
        }
    }

    if (doc.containsKey("effect"))
    {
        _lightEffectSet = true;
        String lightEffect = doc["effect"];
        if (lightEffect == "none")
        {
            _lightEffect = LightEffect::LIGHT_EFFECT_NONE;
        }
        else if (lightEffect == "colorloop")
        {
            _lightEffect = LightEffect::LIGHT_EFFECT_COLORLOOP;
        }
    }

    if (doc.containsKey("transitiontime"))
    {
        _transitionTimeSet = true;
        _transitionTime = doc["transitiontime"];
    }

    if (!doc.containsKey("bri") && doc.containsKey("bri_inc"))
    {
        _brightnessIncrementSet = true;
        _brightnessIncrement = doc["bri_inc"];
    }

    if (!doc.containsKey("hue") && doc.containsKey("hue_inc"))
    {
        _hueIncrementSet = true;
        _hueIncrement = doc["hue_inc"];
    }

    if (!doc.containsKey("sat") && doc.containsKey("sat_inc"))
    {
        _saturationIncrementSet = true;
        _saturationIncrement = doc["sat_inc"];
    }

    if (!doc.containsKey("ct") && doc.containsKey("ct_inc"))
    {
        _colorTemperatureIncrementSet = true;
        _colorTemperatureIncrement = doc["ct_inc"];
    }

    if (!doc.containsKey("xy") && doc.containsKey("xy_inc"))
    {
        _xyPointIncrementSet = true;
        _xyPointIncrement = XYPoint(doc["xy_inc"][0], doc["xy_inc"][1]);
    }
    #else
    String jString = jsonString;
    jString.replace(" ", "");
    jString.replace("\t", "");
    jString.replace("\r", "");
    jString.replace("\n", "");

    int onIndex = jString.indexOf("\"on\":");
    if (onIndex != -1)
    {
        _isOnSet = true;
        _isOn = jString.substring(onIndex + 5, onIndex + 6) == "t";
    }

    int briIndex = jString.indexOf("\"bri\":");
    if (briIndex != -1)
    {
        _brightnessSet = true;
        _brightness = jString.substring(briIndex + 5, jString.indexOf(",", briIndex)).toInt();
    }

    int hueIndex = jString.indexOf("\"hue\":");
    if (hueIndex != -1)
    {
        _hueSet = true;
        _hue = jString.substring(hueIndex + 5, jString.indexOf(",", hueIndex)).toInt();
    }

    int satIndex = jString.indexOf("\"sat\":");
    if (satIndex != -1)
    {
        _saturationSet = true;
        _saturation = jString.substring(satIndex + 5, jString.indexOf(",", satIndex)).toInt();
    }

    int xyIndex = jString.indexOf("\"xy\":");
    if (xyIndex != -1)
    {
        _xyPointSet = true;
        int xyEndIndex = jString.indexOf("]", xyIndex);
        String xyString = jString.substring(xyIndex + 4, xyEndIndex);
        int xyCommaIndex = xyString.indexOf(",");
        _xyPoint = XYPoint(xyString.substring(0, xyCommaIndex).toFloat(), xyString.substring(xyCommaIndex + 1).toFloat());
    }

    int ctIndex = jString.indexOf("\"ct\":");
    if (ctIndex != -1)
    {
        _colorTemperatureSet = true;
        _colorTemperature = jString.substring(ctIndex + 4, jString.indexOf(",", ctIndex)).toInt();
    }

    int alertIndex = jString.indexOf("\"alert\":");
    if (alertIndex != -1)
    {
        _alertEffectSet = true;
        String alertString = jString.substring(alertIndex + 7, jString.indexOf(",", alertIndex));
        if (alertString == "none")
        {
            _alertEffect = AlertEffect::ALERT_EFFECT_NONE;
        }
        else if (alertString == "select")
        {
            _alertEffect = AlertEffect::ALERT_EFFECT_SELECT;
        }
        else if (alertString == "lselect")
        {
            _alertEffect = AlertEffect::ALERT_EFFECT_LSELECT;
        }
    }

    int effectIndex = jString.indexOf("\"effect\":");
    if (effectIndex != -1)
    {
        _lightEffectSet = true;
        String effectString = jString.substring(effectIndex + 8, jString.indexOf(",", effectIndex));
        if (effectString == "none")
        {
            _lightEffect = LightEffect::LIGHT_EFFECT_NONE;
        }
        else if (effectString == "colorloop")
        {
            _lightEffect = LightEffect::LIGHT_EFFECT_COLORLOOP;
        }
    }

    int transitionTimeIndex = jString.indexOf("\"transitiontime\":");
    if (transitionTimeIndex != -1)
    {
        _transitionTimeSet = true;
        _transitionTime = jString.substring(transitionTimeIndex + 16, jString.indexOf(",", transitionTimeIndex)).toInt();
    }

    int briIncIndex = jString.indexOf("\"bri_inc\":");
    if (briIncIndex != -1 && !_brightnessSet)
    {
        _brightnessIncrementSet = true;
        int brightnessIncrement = jString.substring(briIncIndex + 9, jString.indexOf(",", briIncIndex)).toInt();
        _brightnessIncrement = constrain(brightnessIncrement, -254, 254);
    }

    int satIncIndex = jString.indexOf("\"sat_inc\":");
    if (satIncIndex != -1 && !_saturationSet)
    {
        _saturationIncrementSet = true;
        int saturationIncrement = jString.substring(satIncIndex + 9, jString.indexOf(",", satIncIndex)).toInt();
        _saturationIncrement = constrain(saturationIncrement, -254, 254);
    }

    int hueIncIndex = jString.indexOf("\"hue_inc\":");
    if (hueIncIndex != -1 && !_hueSet)
    {
        _hueIncrementSet = true;
        int hueIncrement = jString.substring(hueIncIndex + 9, jString.indexOf(",", hueIncIndex)).toInt();
        _hueIncrement = constrain(hueIncrement, -65534, 65534);
    }

    int ctIncIndex = jString.indexOf("\"ct_inc\":");
    if (ctIncIndex != -1 && !_colorTemperatureSet)
    {
        _colorTemperatureIncrementSet = true;
        int colorTemperatureIncrement = jString.substring(ctIncIndex + 8, jString.indexOf(",", ctIncIndex)).toInt();
        _colorTemperatureIncrement = constrain(colorTemperatureIncrement, -65534, 65534);
    }

    int xyIncIndex = jString.indexOf("\"xy_inc\":");
    if (xyIncIndex != -1 && !_xyPointSet)
    {
        _xyPointIncrementSet = true;
        int xyIncEndIndex = jString.indexOf("]", xyIncIndex);
        String xyIncString = jString.substring(xyIncIndex + 8, xyIncEndIndex);
        int xyIncCommaIndex = xyIncString.indexOf(",");
        _xyPointIncrement = XYPoint(xyIncString.substring(0, xyIncCommaIndex).toFloat(), xyIncString.substring(xyIncCommaIndex + 1).toFloat());
    }
    #endif
}

bool LightStateChange::getIsOn() const {
    return _isOn;
}

bool LightStateChange::isOnSet() const {
    return _isOnSet;
}

[[nodiscard]] uint8_t LightStateChange::getBrightness() const
{
    return _brightness;
}

[[nodiscard]] bool LightStateChange::isBrightnessSet() const
{
    return _brightnessSet;
}

[[nodiscard]] uint16_t LightStateChange::getHue() const
{
    return _hue;
}

[[nodiscard]] bool LightStateChange::isHueSet() const
{
    return _hueSet;
}

[[nodiscard]] uint8_t LightStateChange::getSaturation() const
{
    return _saturation;
}

[[nodiscard]] bool LightStateChange::isSaturationSet() const
{
    return _saturationSet;
}

[[nodiscard]] const XYPoint& LightStateChange::getXyPoint() const
{
    return _xyPoint;
}

[[nodiscard]] bool LightStateChange::isXyPointSet() const
{
    return _xyPointSet;
}

[[nodiscard]] uint16_t LightStateChange::getColorTemperature() const
{
    return _colorTemperature;
}

[[nodiscard]] bool LightStateChange::isColorTemperatureSet() const
{
    return _colorTemperatureSet;
}

[[nodiscard]] AlertEffect LightStateChange::getAlertEffect() const
{
    return _alertEffect;
}

[[nodiscard]] bool LightStateChange::isAlertEffectSet() const
{
    return _alertEffectSet;
}

[[nodiscard]] LightEffect LightStateChange::getLightEffect() const
{
    return _lightEffect;
}

[[nodiscard]] bool LightStateChange::isLightEffectSet() const
{
    return _lightEffectSet;
}

[[nodiscard]] uint16_t LightStateChange::getTransitionTime() const
{
    return _transitionTime;
}

[[nodiscard]] bool LightStateChange::isTransitionTimeSet() const
{

    return _transitionTimeSet;
}

[[nodiscard]] int16_t LightStateChange::getBrightnessIncrement() const
{
    return _brightnessIncrement;
}

[[nodiscard]] bool LightStateChange::isBrightnessIncrementSet() const
{
    return _brightnessIncrementSet;
}

[[nodiscard]] int16_t LightStateChange::getSaturationIncrement() const
{
    return _saturationIncrement;
}

[[nodiscard]] bool LightStateChange::isSaturationIncrementSet() const
{
    return _saturationIncrementSet;
}

[[nodiscard]] int32_t LightStateChange::getHueIncrement() const
{
    return _hueIncrement;
}

[[nodiscard]] bool LightStateChange::isHueIncrementSet() const
{
    return _hueIncrementSet;
}

[[nodiscard]] int32_t LightStateChange::getColorTemperatureIncrement() const
{
    return _colorTemperatureIncrement;
}

[[nodiscard]] bool LightStateChange::isColorTemperatureIncrementSet() const
{
    return _colorTemperatureIncrementSet;
}

[[nodiscard]] const XYPoint& LightStateChange::getXyPointIncrement() const
{
    return _xyPointIncrement;
}

[[nodiscard]] bool LightStateChange::isXyPointIncrementSet() const
{
    return _xyPointIncrementSet;
}

[[nodiscard]] bool LightStateChange::isOnSuccessfullySet() const
{
    return _isOnSuccessfullySet;
}

[[nodiscard]] bool LightStateChange::isBrightnessSuccessfullySet() const
{
    return _brightnessSuccessfullySet;
}

[[nodiscard]] bool LightStateChange::isHueSuccessfullySet() const
{
    return _hueSuccessfullySet;
}

[[nodiscard]] bool LightStateChange::isSaturationSuccessfullySet() const
{
    return _saturationSuccessfullySet;
}

[[nodiscard]] bool LightStateChange::isXyPointSuccessfullySet() const
{
    return _xyPointSuccessfullySet;
}

[[nodiscard]] bool LightStateChange::isColorTemperatureSuccessfullySet() const
{
    return _colorTemperatureSuccessfullySet;
}

[[nodiscard]] bool LightStateChange::isAlertEffectSuccessfullySet() const
{
    return _alertEffectSuccessfullySet;
}

[[nodiscard]] bool LightStateChange::isLightEffectSuccessfullySet() const
{
    return _lightEffectSuccessfullySet;
}

[[nodiscard]] bool LightStateChange::isTransitionTimeSuccessfullySet() const
{
    return _transitionTimeSuccessfullySet;
}

[[nodiscard]] bool LightStateChange::isBrightnessIncrementSuccessfullySet() const
{
    return _brightnessIncrementSuccessfullySet;
}

[[nodiscard]] bool LightStateChange::isSaturationIncrementSuccessfullySet() const
{
    return _saturationIncrementSuccessfullySet;
}

[[nodiscard]] bool LightStateChange::isHueIncrementSuccessfullySet() const
{
    return _hueIncrementSuccessfullySet;
}

[[nodiscard]] bool LightStateChange::isColorTemperatureIncrementSuccessfullySet() const
{
    return _colorTemperatureIncrementSuccessfullySet;
}

[[nodiscard]] bool LightStateChange::isXyPointIncrementSuccessfullySet() const
{
    return _xyPointIncrementSuccessfullySet;
}

void LightStateChange::setOnSuccess(bool success)
{
    _isOnSuccessfullySet = success;
}

void LightStateChange::setBrightnessSuccess(bool success)
{
    _brightnessSuccessfullySet = success;
}

void LightStateChange::setHueSuccess(bool success)
{
    _hueSuccessfullySet = success;
}

void LightStateChange::setSaturationSuccess(bool success)
{
    _saturationSuccessfullySet = success;
}

void LightStateChange::setXyPointSuccess(bool success)
{
    _xyPointSuccessfullySet = success;
}

void LightStateChange::setColorTemperatureSuccess(bool success)
{
    _colorTemperatureSuccessfullySet = success;
}

void LightStateChange::setAlertEffectSuccess(bool success)
{
    _alertEffectSuccessfullySet = success;
}

void LightStateChange::setLightEffectSuccess(bool success)
{
    _lightEffectSuccessfullySet = success;
}

void LightStateChange::setTransitionTimeSuccess(bool success)
{
    _transitionTimeSuccessfullySet = success;
}

void LightStateChange::setBrightnessIncrementSuccess(bool success)
{
    _brightnessIncrementSuccessfullySet = success;
}

void LightStateChange::setSaturationIncrementSuccess(bool success)
{
    _saturationIncrementSuccessfullySet = success;
}

void LightStateChange::setHueIncrementSuccess(bool success)
{
    _hueIncrementSuccessfullySet = success;
}

void LightStateChange::setColorTemperatureIncrementSuccess(bool success)
{
    _colorTemperatureIncrementSuccessfullySet = success;
}

void LightStateChange::setXyPointIncrementSuccess(bool success)
{
    _xyPointIncrementSuccessfullySet = success;
}

/**
 * A response to a successful PUT request contains confirmation of the arguments passed in.
 *
 * Note: If the new value is too large to return in the response due to internal memory constraints then a value of “Updated.” is returned.
 * @return
 */
String LightStateChange::getChanges(int lightId)
{
    String templateString = R"({"success":{"/lights/%d/state/%s":%s}})";
    std::vector<String> changes;
    char buffer[128];
    if(_isOnSet && _isOnSuccessfullySet){
        snprintf(buffer, sizeof(buffer), templateString.c_str(), lightId, "on", _isOn ? "true" : "false");
        changes.emplace_back(buffer);
    }

    if(_brightnessSet && _brightnessSuccessfullySet){
        snprintf(buffer, sizeof(buffer), templateString.c_str(), lightId, "bri", String(_brightness).c_str());
        changes.emplace_back(buffer);
    }

    if(_hueSet && _hueSuccessfullySet){
        snprintf(buffer, sizeof(buffer), templateString.c_str(), lightId, "hue", String(_hue).c_str());
        changes.emplace_back(buffer);
    }

    if(_saturationSet && _saturationSuccessfullySet){
        snprintf(buffer, sizeof(buffer), templateString.c_str(), lightId, "sat", String(_saturation).c_str());
        changes.emplace_back(buffer);
    }

    if(_xyPointSet && _xyPointSuccessfullySet){
        const String& resultVal = "[" + String(_xyPoint.x) + "," + String(_xyPoint.y) + "]";
        snprintf(buffer, sizeof(buffer), templateString.c_str(), lightId, "xy", resultVal.c_str());
        changes.emplace_back(buffer);
    }

    if(_colorTemperatureSet && _colorTemperatureSuccessfullySet){
        snprintf(buffer, sizeof(buffer), templateString.c_str(), lightId, "ct", String(_colorTemperature).c_str());
        changes.emplace_back(buffer);
    }

    if(_alertEffectSet && _alertEffectSuccessfullySet){
        snprintf(buffer, sizeof(buffer), templateString.c_str(), lightId, "alert", AlertEffectToString(_alertEffect));
        changes.emplace_back(buffer);
    }

    if(_lightEffectSet && _lightEffectSuccessfullySet){
        snprintf(buffer, sizeof(buffer), templateString.c_str(), lightId, "effect", LightEffectToString(_lightEffect));
        changes.emplace_back(buffer);
    }

    if(_transitionTimeSet && _transitionTimeSuccessfullySet){
        snprintf(buffer, sizeof(buffer), templateString.c_str(), lightId, "transitiontime", String(_transitionTime).c_str());
        changes.emplace_back(buffer);
    }

    if(_brightnessIncrementSet && _brightnessIncrementSuccessfullySet){
        snprintf(buffer, sizeof(buffer), templateString.c_str(), lightId, "bri_inc", String(_brightnessIncrement).c_str());
        changes.emplace_back(buffer);
    }

    if(_saturationIncrementSet && _saturationIncrementSuccessfullySet){
        snprintf(buffer, sizeof(buffer), templateString.c_str(), lightId, "sat_inc", String(_saturationIncrement).c_str());
        changes.emplace_back(buffer);
    }

    if(_hueIncrementSet && _hueIncrementSuccessfullySet){
        snprintf(buffer, sizeof(buffer), templateString.c_str(), lightId, "hue_inc", String(_hueIncrement).c_str());
        changes.emplace_back(buffer);
    }

    if(_colorTemperatureIncrementSet && _colorTemperatureIncrementSuccessfullySet){
        snprintf(buffer, sizeof(buffer), templateString.c_str(), lightId, "ct_inc", String(_colorTemperatureIncrement).c_str());
        changes.emplace_back(buffer);
    }

    if(_xyPointIncrementSet && _xyPointIncrementSuccessfullySet){
        const String& resultVal = "[" + String(_xyPointIncrement.x) + "," + String(_xyPointIncrement.y) + "]";
        snprintf(buffer, sizeof(buffer), templateString.c_str(), lightId, "xy_inc", resultVal.c_str());
        changes.emplace_back(buffer);
    }

    String result = "";
    for(const auto& change : changes){
        result += change;
        if(&change != &changes.back()){
            result += ",";
        }
    }
    return "[" + result + "]";
}

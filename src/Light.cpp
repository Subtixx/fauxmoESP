#include "Definitions.h"

#include "Light.h"
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

Light::Light(const String& deviceName, uint deviceId)
{
    this->name = deviceName;
    this->deviceId = deviceId;
    String mac = WiFi.macAddress();

    snprintf(uniqueId, DEVICE_UNIQUE_ID_LENGTH, "%s:%s-%02X", mac.c_str(), "00:00", deviceId);
}

String Light::toJson() const
{
    #if USE_ARDUINO_JSON
    StaticJsonDocument<1024> doc;
        doc["name"] = name;
        doc["uniqueid"] = uniqueId;
        doc["state"] = state.toJson();
        doc["capabilities"] = capabilities.toJson();
        String json;
        serializeJson(doc, json);
        return json;
    #else
    return String(R"({"name":")" + name + R"(","uniqueid":")" + uniqueId + R"(","state":)"
            + state.toJson() + R"(,"capabilities":)" + capabilities.toJson() + R"(})");
    #endif
}
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
    this->state = LightState(false);

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
    doc["type"] = type;
    doc["modelid"] = modelid;
    doc["manufacturername"] = manufacturername;
    doc["productname"] = productname;
    doc["swversion"] = swversion;
    String json;
    serializeJson(doc, json);
    return json;
    #else
    // Use format
    char buffer[1024];
    snprintf(buffer, 1024, R"(
        {
            "state": %s,
            "swupdate": {
                "state": "noupdates",
                "lastinstall": "%sT%s"
            },
            "type": "%s",
            "name": "%s",
            "modelid": "%s",
            "manufacturername": "%s",
            "productname": "%s",
            "capabilities": %s,
            "uniqueid": "%s",
            "swversion": "5.105.0.21169"
        }
        )",
            state.toJson().c_str(),
            //2018-01-02T19:24:20
            IsoDate,
            IsoTime,
            type.c_str(),
            name.c_str(),
            modelid.c_str(),
            manufacturername.c_str(),
            productname.c_str(),
            capabilities.toJson().c_str(),
            uniqueId
        );

    return {buffer};
    #endif
}
#include "Light.h"

String Light::toJson() const {
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

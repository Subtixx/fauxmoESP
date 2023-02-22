#include "LightCapabilities.h"

LightCapabilities::LightCapabilities()
        :LightCapabilities(0, 255, 0, 255)
{
}

LightCapabilities::LightCapabilities(uint16_t minDimLevel, uint16_t maxLumen, uint16_t minColorTemperature,
        uint16_t maxColorTemperature)
        :minDimLevel(minDimLevel), maxLumen(maxLumen),
         minColorTemperature(minColorTemperature), maxColorTemperature(maxColorTemperature)
{
}

String LightCapabilities::toJson() const
{
    #if USE_ARDUINO_JSON
    StaticJsonDocument<384> doc;
        doc["mindimlevel"] = minDimLevel;
        doc["maxlumen"] = maxLumen;
        doc["colorgamuttype"] = colorGamutType;
        doc["colorgamut"][0][0] = colorGamut[0].x;
        doc["colorgamut"][0][1] = colorGamut[0].y;
        doc["colorgamut"][1][0] = colorGamut[1].x;
        doc["colorgamut"][1][1] = colorGamut[1].y;
        doc["colorgamut"][2][0] = colorGamut[2].x;
        doc["colorgamut"][2][1] = colorGamut[2].y;
        doc["ct"]["min"] = minColorTemperature;
        doc["ct"]["max"] = maxColorTemperature;

        String json;
        serializeJson(doc, json);
        return json;
    #else
    return String(R"({"mindimlevel":)" + String(minDimLevel) + R"(,"maxlumen":)" + String(maxLumen)
            + R"(,"colorgamuttype":")" + String(colorGamutType) + R"(","colorgamut":[[)"
            + String(colorGamut[0].x) + R"(,)" + String(colorGamut[0].y) + R"(],[)"
            + String(colorGamut[1].x) + R"(,)" + String(colorGamut[1].y) + R"(],[)"
            + String(colorGamut[2].x) + R"(,)" + String(colorGamut[2].y) + R"(]],"ct":{"min":)"
            + String(minColorTemperature) + R"(,"max":)" + String(maxColorTemperature) + R"(}})");
    #endif
}
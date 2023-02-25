#include "LightCapabilities.h"

#if defined(USE_ARDUINO_JSON)
#include <ArduinoJson.h>
#endif

LightCapabilities::LightCapabilities()
        :LightCapabilities(100, 560, 153, 500)
{
}

LightCapabilities::LightCapabilities(uint16_t minDimLevel, uint16_t maxLumen, uint16_t minColorTemperature,
        uint16_t maxColorTemperature)
        :minDimLevel(minDimLevel), maxLumen(maxLumen),
         colorGamutType('C'), colorGamut{ XYPoint(0.6915F, 0.3083F), XYPoint(0.1700F, 0.7000F), XYPoint(0.1532F, 0.0475F) },
         minColorTemperature(minColorTemperature), maxColorTemperature(maxColorTemperature)
{
}

#if defined(USE_ARDUINO_JSON)
StaticJsonDocument<384> LightCapabilities::toJson() const
#else
String LightCapabilities::toJson() const
#endif
{
    #if USE_ARDUINO_JSON
    StaticJsonDocument<384> doc;
    doc["mindimlevel"] = minDimLevel;
    doc["maxlumen"] = maxLumen;
    doc["colorgamuttype"] = String(colorGamutType);
    doc["colorgamut"][0][0] = colorGamut[0].x;
    doc["colorgamut"][0][1] = colorGamut[0].y;
    doc["colorgamut"][1][0] = colorGamut[1].x;
    doc["colorgamut"][1][1] = colorGamut[1].y;
    doc["colorgamut"][2][0] = colorGamut[2].x;
    doc["colorgamut"][2][1] = colorGamut[2].y;
    doc["ct"]["min"] = minColorTemperature;
    doc["ct"]["max"] = maxColorTemperature;
    doc["certified"] = certified;
    doc["streaming"]["renderer"] = streamingRenderer;
    doc["streaming"]["proxy"] = streamingProxy;

    return doc;
    #else
    char buffer[384];
    sprintf(buffer,
            R"({"mindimlevel":%d,"maxlumen":%d,"colorgamuttype":"%c","colorgamut":[[%.3f,%.3f],[%.3f,%.3f],[%.3f,%.3f]],"ct":{"min":%d,"max":%d},"certified":%s,"streaming":{"renderer":%s,"proxy":%s}})",
            minDimLevel, maxLumen, colorGamutType,
            colorGamut[0].x, colorGamut[0].y,
            colorGamut[1].x, colorGamut[1].y,
            colorGamut[2].x, colorGamut[2].y,
            minColorTemperature, maxColorTemperature,
            certified ? "true" : "false",
            streamingRenderer ? "true" : "false",
            streamingProxy ? "true" : "false");

    return { buffer };
    #endif
}
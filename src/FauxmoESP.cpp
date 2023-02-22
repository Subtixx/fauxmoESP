#include <Arduino.h>
#include <ArduinoJson.h>

#include <utility>

#include "FauxmoESP.h"
#include "LightState.h"
#include "Light.h"
#include "LightCapabilities.h"
#include "FauxmoESP.h"
#include "Definitions.h"
#include "templates.h"

FauxmoESP::FauxmoESP()
{
}

FauxmoESP::~FauxmoESP()
{
    teardown();
}

void FauxmoESP::setup(OnStateChangeCallback onStateChangeCallback, OnGetStateCallback onGetStateCallback)
{
    this->_onStateChange = std::move(onStateChangeCallback);
    this->_onGetState = std::move(onGetStateCallback);

    if (this->_webServerEnabled)
    {
        this->webServer = new ESP8266WebServer(_webServerPort);

        // Gets a list of all lights that have been discovered by the bridge.
        this->webServer->on("/api/" + _username + "/lights", HTTP_GET, [this]
        {
            onLightList();
        });

        // Gets the attributes and state of a given light.
        this->webServer->on(UriRegex("/api/" + _username + "/lights/(\\d+)"), HTTP_GET, [this]
        {
            onLightStateRequest();
        });

        // Allows the user to turn the light on and off, modify the hue and effects.
        this->webServer->on(UriRegex("/api/" + _username + "/lights/(\\d+)/state"), HTTP_PUT, [this]
        {
            onLightStateRequest();
        });

        this->webServer->on("/description.xml", HTTP_GET, [this]
        {
            IPAddress ip = WiFi.localIP();
            String mac = WiFi.macAddress();
            mac.replace(":", "");
            mac.toLowerCase();

            char response[strlen_P(FAUXMO_DESCRIPTION_TEMPLATE) + 64];
            snprintf_P(
                    response, sizeof(response),
                    FAUXMO_DESCRIPTION_TEMPLATE,
                    ip[0], ip[1], ip[2], ip[3], _webServerPort,
                    ip[0], ip[1], ip[2], ip[3], _webServerPort,
                    mac.c_str(), mac.c_str()
            );
            webServer->send(200, "text/xml", response);
        });
    }
}

void FauxmoESP::teardown()
{
    for (const auto* light : lights)
    {
        delete light;
    }
    lights.clear();
}

void FauxmoESP::update()
{
    if (webServer != nullptr)
    {
        webServer->handleClient();
    }
    delay(2);
}

/**
 * Gets a list of all lights that have been discovered by the bridge.
 * GET /api/{username}/lights
 */
void FauxmoESP::onLightList()
{
    for (const auto& light : lights)
    {
        Serial.println(light->toJson());
    }
}

/**
 * Gets the name, type and state of a given light.
 * PUT /api/{username}/lights/{id}/state
 */
void FauxmoESP::onLightStateChange()
{
}

/**
 * Gets the name, type and state of a given light.
 * GET /api/{username}/lights/{id}
 */
void FauxmoESP::onLightStateRequest()
{
}

void FauxmoESP::addLight(const String& name, const String& deviceId)
{
    if (getLightByName(name) != nullptr || getLightByDeviceId(deviceId) != nullptr)
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Device with name %s already exists\n", name.c_str())
        return;
    }

    auto* light = new Light();
    light->name = name;
    if (deviceId.length() >= DEVICE_UNIQUE_ID_LENGTH)
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Device ID too long: %s\n", deviceId.c_str())
    }

    deviceId.toCharArray(light->uniqueId, DEVICE_UNIQUE_ID_LENGTH);
    lights.push_back(light);
}

void FauxmoESP::removeLight(const String& deviceId)
{
    const auto result = std::find_if(lights.begin(), lights.end(), [&deviceId](const auto* light)
    {
        return light->uniqueId == deviceId.c_str();
    });

    if (result == lights.end())
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Device with ID %s not found\n", deviceId.c_str())
        return;
    }

    delete *result;
    lights.erase(result);
}

Light* FauxmoESP::getLightByName(const String& name)
{
    const auto result = std::find_if(lights.begin(), lights.end(), [&name](const auto* light)
    {
        return light->name == name;
    });

    if (result == lights.end())
    {
        return nullptr;
    }

    return *result;
}

Light* FauxmoESP::getLightByDeviceId(const String& deviceId)
{
    const auto result = std::find_if(lights.begin(), lights.end(), [&deviceId](const auto* light)
    {
        return light->uniqueId == deviceId.c_str();
    });

    if (result == lights.end())
    {
        return nullptr;
    }

    return *result;
}

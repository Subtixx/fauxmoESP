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
            onLightStateChange();
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
            webServer->send(200, "text/xml", String(response));
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
 *
 * @note A light cannot have its hue, saturation, brightness, effect, ct or xy modified when it is turned off.
 *       Doing so will return 201 error.
 *       There are 3 methods available to set the color of the light – hue and saturation (hs), xy or color temperature (ct).
 *       If multiple methods are used then a priority is used: xy > ct > hs. All included parameters will be updated
 *       but the ‘colormode’ will be set using the priority system.
 */
void FauxmoESP::onLightStateChange()
{
    String username = webServer->pathArg(0);
    String lightId = webServer->pathArg(1);

    auto* light = getLightByDeviceId(lightId);
    if (light == nullptr)
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Light with id %s not found\n", lightId.c_str())
        webServer->send(404);
        return;
    }

    if (!webServer->hasArg("plain"))
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "No body\n")
        webServer->send(400);
        return;
    }

    String body = webServer->arg("plain");
    LightState state = LightState::fromJson(body);

    // Modifying hue, saturation, brightness, effect, ct or xy when the light is off will return 201 error.
    if (!light->state.isValidChange(state))
    {
        DEBUG_MSG_FAUXMO(
                FAUXMO_LOG_TAG "Modifying hue, saturation, brightness, effect, ct or xy when the light is off is not allowed\n")
        webServer->send(201);
        return;
    }

    if (state.isLightReachable)
    {
        light->state = state;
        if (_onStateChange != nullptr)
        {
            this->_onStateChange(light);
        }
        webServer->send(200);
        return;
    }
    else
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "State is not valid\n")
        webServer->send(400);
        return;
    }

    assert(false && "Should not be possible to reach this point");
}

/**
 * Gets the name, type and state of a given light.
 * GET /api/{username}/lights/{id}
 */
void FauxmoESP::onLightStateRequest()
{
    String username = webServer->pathArg(0);
    String lightId = webServer->pathArg(1);

    auto* light = getLightByDeviceId(lightId);
    if (light == nullptr)
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Light with id %s not found\n", lightId.c_str())
        webServer->send(404);
        return;
    }

    if (_onGetState != nullptr)
    {
        this->_onGetState(light);
    }

    webServer->send(200, "application/json", light->toJson());
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

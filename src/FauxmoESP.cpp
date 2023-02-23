#include <Arduino.h>

#if USE_ARDUINO_JSON
#include <ArduinoJson.h>
#endif

#include <utility>
#include <ESP8266SSDP.h>

#include "FauxmoESP.h"
#include "LightState.h"
#include "Light.h"
#include "Definitions.h"
#include "LightStateChange.h"

FauxmoESP::FauxmoESP()
        :webServer(nullptr)
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

    createSsdp();

    if (this->_webServerEnabled)
    {
        this->webServer = new ESP8266WebServer(_webServerPort);

        // Allows the user to turn the light on and off, modify the hue and effects.
        this->webServer->on(UriRegex("/api/([A-Za-z0-9]+)/lights/([0-9]+)/state"), HTTP_PUT, [this]
        {
            String username = webServer->pathArg(0);
            String lightId = webServer->pathArg(1);
            if (!checkUsername(username))
            {
                return;
            }

            DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Received /api/%s/lights/%s/state request!\n", username.c_str(),
                    lightId.c_str())
            onLightStateChange();
        });

        // Gets the attributes and state of a given light.
        this->webServer->on(UriRegex("/api/([A-Za-z0-9]+)/lights/([0-9]+)"), HTTP_GET, [this]
        {
            String username = webServer->pathArg(0);
            String lightId = webServer->pathArg(1);
            if (!checkUsername(username))
            {
                return;
            }

            DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Received /api/%s/lights/%s request!\n", username.c_str(), lightId.c_str())
            onLightStateRequest();
        });

        // Gets a list of all lights that have been discovered by the bridge.
        this->webServer->on(UriRegex("/api/([A-Za-z0-9]+)/lights"), HTTP_GET, [this]
        {
            String username = webServer->pathArg(0);
            if (!checkUsername(username))
            {
                return;
            }

            DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Received /api/%s/lights request!\n", username.c_str())
            onLightList();
        });

        this->webServer->on("/description.xml", HTTP_GET, [this]
        {
            SSDP.schema(webServer->client());
        });

        this->webServer->on("/api", HTTP_POST, [this]
        {
            DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Received /api POST request!\n")
            webServer->send(200, "text/plain", R"([{"success":{"username":")" + _username + "\"}}]");
        });

        this->webServer->onNotFound([this]
        {
            auto method = "GET";
            if (webServer->method() == HTTP_POST)
            {
                method = "POST";
            }
            else if (webServer->method() == HTTP_PUT)
            {
                method = "PUT";
            }
            else if (webServer->method() == HTTP_DELETE)
            {
                method = "DELETE";
            }
            DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Received unknown request! %s %s\n", method, webServer->uri().c_str())
            webServer->send(404, "text/plain", "Not found");
        });

        this->webServer->begin();
    }
}
void FauxmoESP::createSsdp() const
{
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    mac.toLowerCase();

    MDNS.addService("ssdp", "udp", 1900);
    MDNS.addService("upnp", "tcp", 1900);

    // Create SSDP philips hue device
    SSDP.setSchemaURL("description.xml");
    SSDP.setHTTPPort(_webServerPort);
    SSDP.setName("Philips hue (" + mac + ")");
    SSDP.setURL("/");
    SSDP.setModelName("Philips hue bridge 2012");
    SSDP.setModelNumber("929000226503");
    SSDP.setModelURL("http://www.meethue.com");
    SSDP.setManufacturer("Royal Philips Electronics");
    SSDP.setManufacturerURL("http://www.philips.com");
    SSDP.setDeviceType("urn:schemas-upnp-org:device:Basic:1");
    SSDP.setSerialNumber(mac.c_str());
    SSDP.begin();
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
    String response = "{";
    auto i = 1;
    for (auto& light : lights)
    {
        if (_onGetState != nullptr)
        {
            this->_onGetState(light);
        }

        response += "\"" + String(i) + "\": " + light->toJson();
        if (light != lights.back())
        {
            response += ",";
        }
        i++;
    }

    response += "}";

    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Sending light list!\n")
    webServer->send(200, "application/json", response);
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
    ushort lightId = webServer->pathArg(1).toInt();

    auto* light = getLightByDeviceId(lightId);
    if (light == nullptr)
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Light with id %d not found\n", lightId)
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
    auto stateChange = LightStateChange(body);
    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Received state change: %s (%s)\n", stateChange.getChanges(lightId).c_str(), body.c_str())

    // Modifying hue, saturation, brightness, effect, ct or xy when the light is off will return 201 error.
    if (!light->state.isOn && (!stateChange.isOnSet() || !stateChange.getIsOn()))
    {
        DEBUG_MSG_FAUXMO(
                FAUXMO_LOG_TAG "Modifying hue, saturation, brightness, effect, ct or xy when the light is off is not allowed\n")
        webServer->send(201);
        return;
    }

    if (light->state.isLightReachable)
    {
        if (_onStateChange != nullptr)
        {
            this->_onStateChange(light, &stateChange);
        }

        const String& changesMade = stateChange.getChanges(lightId);
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Light %s State changed: %s!\n", light->name.c_str(), changesMade.c_str())
        webServer->send(200, "application/json", changesMade);
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
    ushort lightId = webServer->pathArg(1).toInt();

    auto* light = getLightByDeviceId(lightId);
    if (light == nullptr)
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Light with id %d not found\n", lightId)
        webServer->send(404);
        return;
    }

    if (_onGetState != nullptr)
    {
        this->_onGetState(light);
    }

    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Sending state for light %s\n", light->name.c_str())
    webServer->send(200, "application/json", light->toJson());
}

Light* FauxmoESP::addLight(const String& name)
{
    if (getLightByName(name) != nullptr)
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Device with name %s already exists\n", name.c_str())
        return nullptr;
    }

    if (lights.size() >= MAX_DEVICES)
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Maximum number of devices reached\n")
        return nullptr;
    }

    auto* light = new Light(name, lights.size() + 1);
    lights.push_back(light);

    return light;
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

Light* FauxmoESP::getLightByDeviceId(const ushort deviceId)
{
    const auto result = std::find_if(lights.begin(), lights.end(), [&deviceId](const auto* light)
    {
        return light->deviceId == deviceId;
    });

    if (result == lights.end())
    {
        return nullptr;
    }

    return *result;
}

bool FauxmoESP::checkUsername(const String& username)
{
    if (this->_checkUsername && username != this->_username)
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Unauthorized user %s\n", username.c_str())
        webServer->send(403, "application/json",
                R"({"error":{"type":1,"address":"","description":"unauthorized user"}})");
        return false;
    }

    return true;
}
Light* FauxmoESP::addLight(const String& name, const LightState& initialState)
{
    auto* light = addLight(name);
    if (light == nullptr)
    {
        return nullptr;
    }

    light->state = initialState;

    return light;
}

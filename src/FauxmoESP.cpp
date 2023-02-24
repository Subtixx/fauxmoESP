#include <Arduino.h>

#include <utility>

#include "FauxmoESP.h"
#include "LightState.h"
#include "Light.h"
#include "Definitions.h"
#include "LightStateChange.h"

FauxmoESP::FauxmoESP(bool enableWebServer)
        :webServer(nullptr), _webServerEnabled(enableWebServer)
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
        setupWebServer();
    }
}

void FauxmoESP::processRequest(const String& url, const String& body, const String& method, String& response, uint16_t* resultCode)
{
    // GET /api/<username>/lights
    if (method == "GET" && url.startsWith("/api/") && url.endsWith("/lights"))
    {
        onLightList(response, resultCode);
    }

    // PUT /api/<username>/lights/<lightId>/state
    else if (method == "PUT" && url.startsWith("/api/") && url.endsWith("/state"))
    {
        String username = url.substring(5, url.indexOf("/lights/"));
        String lightId = url.substring(url.indexOf("/lights/") + 8, url.indexOf("/state"));

        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Received /api/%s/lights/%s/state request!\n", username.c_str(),
                lightId.c_str())

        onLightStateChange(lightId.toInt(), body, response, resultCode);
    }

    // GET /api/<username>/lights/<lightId>
    else if (method == "GET" && url.startsWith("/api/") && url.endsWith("/state"))
    {
        String username = url.substring(5, url.indexOf("/lights/"));
        String lightId = url.substring(url.indexOf("/lights/") + 8, url.indexOf("/state"));

        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Received /api/%s/lights/%s/state request!\n", username.c_str(),
                lightId.c_str())

        onLightStateRequest(lightId.toInt(), response, resultCode);
    }
    // GET /description.xml
    else if (method == "GET" && url == "/description.xml")
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Received /description.xml request!\n")
    #if defined(ESP8266)
    #elif defined(ESP32)
        response = SSDP.getSchema();
    #endif
        *resultCode = 200;
    }
    // POST /api
    else if (method == "POST" && url == "/api")
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Received /api request!\n")
        response = R"({"success":{"username":")" + this->_username + R"("}})";
        *resultCode = 200;
    }
    else
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Received unknown request!\n")
        *resultCode = 404;
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

void FauxmoESP::setupWebServer()
{
    #if defined(ESP8266)
    this->webServer = new ESP8266WebServer(_webServerPort);
    #elif defined(ESP32)
    this->webServer = new WebServer(_webServerPort);
    #endif

    // Allows the user to turn the light on and off, modify the hue and effects.
    this->webServer->on(UriRegex("/api/([A-Za-z0-9]+)/lights/([0-9]+)/state"), HTTP_PUT, [this]
    {
        String username = webServer->pathArg(0);
        String lightId = webServer->pathArg(1);
        if (!checkUsername(username))
        {
            return;
        }

        if (!webServer->hasArg("plain"))
        {
            DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "No body\n")
            webServer->send(400, "application/json", R"({"error":{"type":3,"address":"/lights/)" + String(lightId) + R"(","description":"resource, /lights/)" +
                    String(lightId) + ", not available\"}}");
            return;
        }
        String body = webServer->arg("plain");

        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Received /api/%s/lights/%s/state request!\n", username.c_str(),
                lightId.c_str())

        String response;
        uint16_t resultCode = 0;
        onLightStateChange(lightId.toInt(), body, response, &resultCode);

        webServer->send(resultCode, "application/json", response);
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
        String response;
        uint16_t resultCode = 0;
        onLightStateRequest(lightId.toInt(), response, &resultCode);

        webServer->send(resultCode, "application/json", response);
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
        String response;
        uint16_t resultCode = 0;
        onLightList(response, &resultCode);

        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Sending light list!\n")
        webServer->send(resultCode, "application/json", response);
    });

    this->webServer->on("/description.xml", HTTP_GET, [this]
    {
    #if defined(ESP8266)
        SSDP.schema(webServer->client());
    #elif defined(ESP32)
        webServer->send(200, "text/xml", SSDP.getSchema());
    #endif
    });

    this->webServer->on("/api", HTTP_POST, [this]
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Received /api POST request!\n")
        webServer->send(200, "application/json", R"([{"success":{"username":")" + _username + "\"}}]");
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

void FauxmoESP::update()
{
    if (webServer != nullptr)
    {
        webServer->handleClient();
    }

    delay(2);
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

/**
 * Gets a list of all lights that have been discovered by the bridge.
 * GET /api/{username}/lights
 */
void FauxmoESP::onLightList(String& response, uint16_t* resultCode)
{
    response = "{";
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
    *resultCode = 200;
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
void FauxmoESP::onLightStateChange(uint16_t lightId, const String& body, String& response, uint16_t* resultCode)
{
    auto* light = getLightByDeviceId(lightId);
    if (light == nullptr)
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Light with id %d not found\n", lightId)
        response = R"({"error":{"type":3,"address":"/lights/)" + String(lightId) + R"(","description":"resource, /lights/)" +
                String(lightId) + ", not available\"}}";
        *resultCode = 404;
        return;
    }

    auto stateChange = LightStateChange(body);
    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Received state change: %s (%s)\n", stateChange.getChanges(lightId).c_str(),
            body.c_str())

    // Modifying hue, saturation, brightness, effect, ct or xy when the light is off will return 201 error.
    if (!light->state.isOn && (!stateChange.isOnSet() || !stateChange.getIsOn()))
    {
        DEBUG_MSG_FAUXMO(
                FAUXMO_LOG_TAG "Modifying hue, saturation, brightness, effect, ct or xy when the light is off is not allowed\n")
        response = R"({"error":{"type": 201, "address": "/lights/)" + String(lightId)
                + R"(/state", "description": "Modifying hue, saturation, brightness, effect, ct or xy when the light is off is not allowed."}})";
        *resultCode = 201;
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
        response = changesMade;
        *resultCode = 200;
        return;
    }
    else
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "State is not valid\n")
        response = R"({"error":{"type": 201, "address": "/lights/)" + String(lightId)
                + R"(/state", "description": "State is not valid."}})";
        *resultCode = 400;
        return;
    }

    assert(false && "Should not be possible to reach this point");
}

/**
 * Gets the name, type and state of a given light.
 * GET /api/{username}/lights/{id}
 */
void FauxmoESP::onLightStateRequest(uint16_t lightId, String& response, uint16_t* resultCode)
{
    auto* light = getLightByDeviceId(lightId);
    if (light == nullptr)
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Light with id %d not found\n", lightId)
        response = R"({"error":{"type":3,"address":"/lights/)" + String(lightId) + R"(","description":"resource, /lights/)" +
                String(lightId) + ", not available\"}}";
        *resultCode = 404;
        return;
    }

    if (_onGetState != nullptr)
    {
        this->_onGetState(light);
    }

    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Sending state for light %s\n", light->name.c_str())
    response = light->toJson();
    *resultCode = 200;
}

Light* FauxmoESP::addLight(const String& name, const LightCapabilities& capabilities, const LightState& initialState)
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
    light->state = initialState;
    light->capabilities = capabilities;
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
    const auto result = std::find_if(lights.begin(), lights.end(),
            [&deviceId](const Light* light)
            {
                return light->deviceId == deviceId;
            });

    if (result == lights.end())
    {
        return nullptr;
    }

    return *result;
}
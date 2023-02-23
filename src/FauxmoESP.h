#pragma once

#include "Light.h"
#include "LightState.h"

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <uri/UriBraces.h>
#include <uri/UriRegex.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#endif

class FauxmoESP
{
private:
    std::vector<Light*> lights;

    void onLightList();
    void onLightStateChange();
    void onLightStateRequest();

    uint16_t _webServerPort = 80;

    OnStateChangeCallback _onStateChange;
    OnGetStateCallback _onGetState;

    ESP8266WebServer* webServer;

    String _username = "2WLEDHardQrI3WHYTHoMcXHgEspsM8ZZRpSKtBQr";
    bool _webServerEnabled = true;
public:
    FauxmoESP();
    ~FauxmoESP();

    void setup(
            OnStateChangeCallback onStateChangeCallback,
            OnGetStateCallback onGetStateCallback
    );

    void teardown();
    void update();

    void setUsername(const String& username)
    {
        this->_username = username;
    }

    void setWebServerPort(uint16_t port)
    {
        this->_webServerPort = port;
    }

    void setWebServerEnabled(bool enabled)
    {
        this->_webServerEnabled = enabled;
    }

    Light* addLight(const String& name);
    void removeLight(const String& deviceId);

    Light* getLightByDeviceId(const String& deviceId);
    Light* getLightByName(const String& name);
};
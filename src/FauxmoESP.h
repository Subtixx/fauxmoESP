#pragma once

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266SSDP.h>
#include <ESP8266WebServer.h>

#include <WiFiUdp.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ESP32SSDP.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
    #include <AsyncTCP_RP2040W.h>
#else
    #error Platform not supported
#endif

#include <uri/UriBraces.h>
#include <uri/UriRegex.h>

#include <vector>
#include "Light.h"
#include "LightState.h"

class FauxmoESP
{
private:
    std::vector<Light*> lights;

    void onLightList(String& response, uint16_t* resultCode);
    void onLightStateChange(uint16_t lightId, const String& body, String& response, uint16_t* resultCode);
    void onLightStateRequest(uint16_t lightId, String& response, uint16_t* resultCode);

    OnStateChangeCallback _onStateChange;
    OnGetStateCallback _onGetState;

    #if defined(ESP8266)
        ESP8266WebServer* webServer;
    #elif defined(ESP32)
        WebServer* webServer;
    #endif
    bool _webServerEnabled = true;
    bool _checkUsername = true;
    void setupWebServer();

    String _username = "2WLEDHardQrI3WHYTHoMcXHgEspsM8ZZRpSKtBQr";
    uint16_t _webServerPort = 80;
public:
    explicit FauxmoESP(bool enableWebServer = true);
    ~FauxmoESP();

    void setup(
            OnStateChangeCallback onStateChangeCallback,
            OnGetStateCallback onGetStateCallback
    );

    void teardown();

    Light* addLight(const String& name, const LightCapabilities& capabilities = LightCapabilities(), const LightState& initialState = LightState());
    void removeLight(const String& deviceId);

    void update();

    void setWebServerEnabled(bool enabled)
    {
        this->_webServerEnabled = enabled;
    }

    void setCheckUsername(bool checkUsername)
    {
        this->_checkUsername = checkUsername;
    }

    bool checkUsername(const String& username);
    void processRequest(const String& url, const String& body, const String& method, String& response, uint16_t* resultCode);

    void setUsername(const String& username)
    {
        this->_username = username;
    }

    void setWebServerPort(uint16_t port)
    {
        this->_webServerPort = port;
    }
    Light* getLightByDeviceId(ushort deviceId);
    Light* getLightByName(const String& name);
    void createSsdp() const;
};
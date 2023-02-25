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

/**
 * @brief Main class for FauxmoESP
 */
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

    void createSsdp() const;
public:

    /**
     * @brief Construct a new FauxmoESP object
     * @param enableWebServer Whether to enable the web server or not
     */
    explicit FauxmoESP(bool enableWebServer = true);

    /**
     * @brief Destroy the FauxmoESP object
     */
    ~FauxmoESP();

    /**
     * @brief Setup the FauxmoESP object
     * @param onStateChangeCallback Callback to be called when a device state changes
     * @param onGetStateCallback Callback to be called when a device state is requested
     */
    void setup(
            OnStateChangeCallback onStateChangeCallback,
            OnGetStateCallback onGetStateCallback
    );

    /**
     * @brief Teardown the FauxmoESP object
     */
    void teardown();

    /**
     * @brief Add a light to the FauxmoESP object
     * @param name The name of the light
     * @param capabilities The capabilities of the light, defaults to LightCapabilities()
     * @param initialState The initial state of the light, defaults to LightState()
     * @return Light* The light object or nullptr if the light could not be added
     */
    Light* addLight(const String& name, const LightCapabilities& capabilities = LightCapabilities(), const LightState& initialState = LightState());

    /**
     * @brief Remove a light from the FauxmoESP object
     * @param deviceId The device id of the light
     */
    void removeLight(const String& deviceId);

    /**
     * @brief Handles the SSDP and web server
     */
    void update();

    /**
     * @brief Set whether the web server is enabled or not
     * @param enabled Whether the web server is enabled or not
     */
    void setWebServerEnabled(bool enabled)
    {
        this->_webServerEnabled = enabled;
    }

    /**
     * @brief Set whether the username should be checked or not
     * If a request is made with a username that does not match the known one, the request will be denied
     *
     * @param checkUsername Whether the username should be checked or not
     */
    void setCheckUsername(bool checkUsername)
    {
        this->_checkUsername = checkUsername;
    }

    /**
     * @brief Check if the username is correct
     * @param username The username to check
     * @return true If the username is correct, false otherwise
     */
    bool checkUsername(const String& username);

    /**
     * @brief Process a request, only used when the web server is disabled.
     * @param url The full URL of the request
     * @param body The body of the request
     * @param method The method of the request
     * @param response Returns the response
     * @param resultCode Returns the result code
     */
    void processRequest(const String& url, const String& body, const String& method, String& response, uint16_t* resultCode);

    /**
     * @brief Set the username
     * @param username The username
     */
    void setUsername(const String& username)
    {
        this->_username = username;
    }

    /**
     * @brief Set the web server port
     * @param port The web server port
     */
    void setWebServerPort(uint16_t port)
    {
        this->_webServerPort = port;
    }

    /**
     * @brief Get a light by its device id
     * @param deviceId The device id of the light
     * @return Light* The light object or nullptr if the light could not be found
     */
    Light* getLightByDeviceId(ushort deviceId);

    /**
     * @brief Get a light by its name
     * @param name The name of the light
     * @return Light* The light object or nullptr if the light could not be found
     */
    Light* getLightByName(const String& name);
};
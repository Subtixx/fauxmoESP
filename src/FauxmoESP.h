/*

FAUXMO ESP

Copyright (C) 2016-2020 by Xose Pérez <xose dot perez at gmail dot com>

The MIT License (MIT)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#pragma once

#define FAUXMO_UDP_MULTICAST_IP     IPAddress(239,255,255,250)
#define FAUXMO_UDP_MULTICAST_PORT   1900
#define FAUXMO_TCP_MAX_CLIENTS      10
#define FAUXMO_TCP_PORT             1901
#define FAUXMO_RX_TIMEOUT           3
#define FAUXMO_DEVICE_UNIQUE_ID_LENGTH  27
#define FAUXMO_MAX_DEVICES          2147483646
#define FAUXMO_LOG_TAG            "[FAUXMO] "

//#define DEBUG_FAUXMO                Serial
#ifdef DEBUG_FAUXMO
    #if defined(ARDUINO_ARCH_ESP32)
        #define DEBUG_MSG_FAUXMO(fmt, ...) { DEBUG_FAUXMO.printf_P((PGM_P) PSTR(fmt), ## __VA_ARGS__); }
    #else
        #define DEBUG_MSG_FAUXMO(fmt, ...) { DEBUG_FAUXMO.printf(fmt, ## __VA_ARGS__); }
    #endif
#else
    #define DEBUG_MSG_FAUXMO(...) {}
#endif

#ifndef DEBUG_FAUXMO_VERBOSE_TCP
#define DEBUG_FAUXMO_VERBOSE_TCP    false
#endif

#ifndef DEBUG_FAUXMO_VERBOSE_UDP
#define DEBUG_FAUXMO_VERBOSE_UDP    false
#endif

#include <Arduino.h>

#if defined(ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESPAsyncTCP.h>
#elif defined(ESP32)
    #include <WiFi.h>
    #include <AsyncTCP.h>
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
    #include <AsyncTCP_RP2040W.h>
#else
    #error Platform not supported
#endif

#include <WiFiUdp.h>

#include <functional>
#include <vector>
#include "templates.h"

typedef struct
{
    String name;

    bool state;
    unsigned char value;
    unsigned char hue;
    unsigned char sat;
    char uniqueid[FAUXMO_DEVICE_UNIQUE_ID_LENGTH];

} fauxmoesp_device_t;

typedef std::function<void(unsigned int, fauxmoesp_device_t*)> TSetStateCallback;

class FauxmoESP
{
public:
    ~FauxmoESP();

    /**
     * Add a device to the list of devices that can be controlled.
     * @param deviceName The name of the device
     * @return The id of the device
     */
    [[maybe_unused]] unsigned char addDevice(const char* deviceName);

    /**
     * Rename a device by ID.
     * @param id The ID of the device to rename
     * @param newDeviceName The new name of the device
     * @return True if the device was renamed, false if the device was not found
     */
    [[maybe_unused]] bool renameDevice(unsigned char id, const String& newDeviceName);

    /**
     * Rename a device by name
     * @param oldDeviceName The name of the device to rename.
     * @param newDeviceName The new name of the device
     * @return True if the device was renamed, false if the device was not found
     */
    [[maybe_unused]] bool renameDevice(const char* oldDeviceName, const char* newDeviceName);

    /**
     * Remove a device from the list of devices that can be controlled, by ID.
     * @param id The ID of the device to remove
     * @return True if the device was removed, false if the device was not found
     */
    [[maybe_unused]] bool removeDevice(unsigned char id);

    /**
     * Remove a device from the list of devices that can be controlled, by name.
     * @param deviceName The name of the device to remove
     * @return True if the device was removed, false if the device was not found
     */
    [[maybe_unused]] bool removeDevice(const char* deviceName);

    /**
     * Retrieve the name of the device with the given ID.
     * @param id The ID of the device
     * @return The name of the device
     */
    [[maybe_unused]] String getDeviceName(unsigned char id);

    /**
     * Retrieve the ID of the device with the given name.
     * @param deviceName The name of the device
     * @return The ID of the device
     */
    [[maybe_unused]] unsigned int getDeviceId(const String& deviceName);

    /**
     * Set the unique identifier of the device with the given ID.
     * @param id The ID of the device
     * @param uniqueId The unique identifier to set the device to
     */
    [[maybe_unused]] void setDeviceUniqueId(unsigned char id, const char* uniqueId);

    /**
     * @brief Get the state of a device
     *
     * @param id The id of the device
     * @param state The state of the device
     * @param value The value of the device
     * @param hue The hue of the device
     * @param sat The saturation of the device
     * @return true The device was found and the state was returned
     * @return false The device was not found
     */
    [[maybe_unused]] bool setState(unsigned char id, bool state, unsigned char value, unsigned char hue, unsigned char sat);

    /**
     * @brief Get the state of a device
     *
     * @param deviceName The name of the device
     * @param state The state of the device
     * @param value The value of the device
     * @param hue The hue of the device
     * @param sat The saturation of the device
     * @return true The device was found and the state was returned
     * @return false The device was not found
     */
    [[maybe_unused]] bool setState(const char* deviceName, bool state, unsigned char value, unsigned char hue, unsigned char sat);

    /**
     * @brief Process a TCP request
     * @param client TCP client
     * @param isGet True if the request is a GET
     * @param url URL of the request
     * @param body Body of the request
     * @return True if the request was processed
     */
    [[maybe_unused]] bool process(AsyncClient* client, bool isGet, const String& url, const String& body);

    /**
     * @brief Enable or disable fauxmoESP
     * @param enable True to enable, false to disable
     */
    [[maybe_unused]] void enable(bool enable);

    /**
     * @brief Main loop function to handle UDP and TCP requests
     */
    void handle();

    /**
     * Set the callback function to be called when a device state is changed.
     * @param fn The callback function
     */
    [[maybe_unused]] void onSetState(TSetStateCallback fn) { _setCallback = fn; }

    /**
     * Set whether the integrated web server is enabled or not.
     * If not enabled you must handle the requests yourself and call handleHttpRequest in your function.
     * @param enabled True to enable, false to disable
     */
    [[maybe_unused]] void setWebServerEnabled(bool enabled) { _internalWebServerEnabled = enabled; }

    /**
     * Set the TCP port for the integrated web server. Default is 80.
     * @param tcp_port
     */
    [[maybe_unused]] void setWebServerPort(unsigned long tcp_port) { _tcp_port = tcp_port; }

    /**
     *
     * @param redirect_port
     */
    [[maybe_unused]] void setWebServerRedirect(unsigned long redirect_port) { _redirect_port = redirect_port; }
private:
    AsyncServer* _server = nullptr;

    bool _fauxmoEnabled = false;
    bool _internalWebServerEnabled = true;
    unsigned int _tcp_port = FAUXMO_TCP_PORT;
    unsigned int _redirect_port = 0;
    
    std::vector<fauxmoesp_device_t> _devices;
    #ifdef ESP8266
    WiFiEventHandler _handler;
    #endif
    WiFiUDP _udp;
    AsyncClient* _tcpClients[FAUXMO_TCP_MAX_CLIENTS];
    TSetStateCallback _setCallback = NULL;

    String _deviceJson(unsigned char id,
            bool all);    // all = true means we are listing all devices so use full description template

    void _handleUDP();
    [[maybe_unused]] void _onUDPData(const IPAddress& remoteIP, unsigned int remotePort, void* data, size_t len);
    void _sendUDPResponse();

    void _onTCPClient(AsyncClient* client);
    bool _onTCPData(AsyncClient* client, void* data, size_t len);
    bool _onTCPRequest(AsyncClient* client, bool isGet, const String& url, const String& body);
    bool _onTCPDescription(AsyncClient* client, [[maybe_unused]] const String& url,
            [[maybe_unused]] const String& body) const;
    bool _onTCPList(AsyncClient* client, const String& url, [[maybe_unused]] const String& body);
    bool _onTCPControl(AsyncClient* client, const String& url, const String& body);
    static void _sendTCPResponse(AsyncClient* client, const char* code, const char* body, const char* mime);
};

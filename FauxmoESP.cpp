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

#include <Arduino.h>
#include "FauxmoESP.h"
#include <ArduinoJson.h>

FauxmoESP::~FauxmoESP()
{
    // Delete devices
    _devices.clear();
}

/**
 * Respond to a UDP M-SEARCH request
 * \see https://en.wikipedia.org/wiki/Simple_Service_Discovery_Protocol
 */
void FauxmoESP::_sendUDPResponse()
{
    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Responding to M-SEARCH request\n")

    IPAddress ip = WiFi.localIP();
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    mac.toLowerCase();

    char response[strlen(FAUXMO_UDP_RESPONSE_TEMPLATE) + 128];
    snprintf_P(
            response, sizeof(response),
            FAUXMO_UDP_RESPONSE_TEMPLATE,
            ip[0], ip[1], ip[2], ip[3],
            _tcp_port,
            mac.c_str(), mac.c_str()
    );

    #if DEBUG_FAUXMO_VERBOSE_UDP
    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "UDP response sent to %s:%d\n%s", _udp.remoteIP().toString().c_str(), _udp.remotePort(), response);
    #endif

    _udp.beginPacket(_udp.remoteIP(), _udp.remotePort());
    #if defined(ESP32)
    _udp.printf(response);
    #else
    _udp.write(response);
    #endif
    _udp.endPacket();

}

void FauxmoESP::_handleUDP()
{
    int len = _udp.parsePacket();
    if (len > 0)
    {
        unsigned char data[len + 1];
        _udp.read(data, len);
        data[len] = 0;

        #if DEBUG_FAUXMO_VERBOSE_UDP
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "UDP packet received\n%s", (const char *) data);
        #endif

        String request = String((const char*)data);
        if (request.indexOf("M-SEARCH") >= 0)
        {
            if ((request.indexOf("ssdp:discover") > 0) || (request.indexOf("upnp:rootdevice") > 0)
                    || (request.indexOf("device:basic:1") > 0))
            {
                _sendUDPResponse();
            }
        }
    }
}


// -----------------------------------------------------------------------------
// TCP
// -----------------------------------------------------------------------------
/**
 * Respond to a TCP request
 * @param client TCP client
 * @param code HTTP code
 * @param body HTTP body
 * @param mime HTTP mime type
 */
void FauxmoESP::_sendTCPResponse(AsyncClient* client, const char* code, const char* body, const char* mime)
{
    char headers[strlen_P(FAUXMO_TCP_HEADERS) + 32];
    snprintf_P(
            headers, sizeof(headers),
            FAUXMO_TCP_HEADERS,
            code, mime, strlen(body)
    );

    #if DEBUG_FAUXMO_VERBOSE_TCP
    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Response:\n%s%s\n", headers, body);
    #endif

    client->write(headers);
    client->write(body);
}

/*
 * Respond with device information
 * @param id Device ID
 * @param all Include all information (true) or just the name and unique ID (false)
 */
String FauxmoESP::_deviceJson(unsigned char id, bool all = true)
{

    if (id >= _devices.size()) return "{}";

    fauxmoesp_device_t device = _devices[id];

    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Sending device info for \"%s\", uniqueID = \"%s\"\n", device.name.c_str(),
            device.uniqueid)
    char buffer[strlen_P(FAUXMO_DEVICE_JSON_TEMPLATE) + 64];

    if (all)
    {
        snprintf_P(
                buffer, sizeof(buffer),
                FAUXMO_DEVICE_JSON_TEMPLATE,
                device.name.c_str(),
                device.uniqueid,
                device.state ? "true" : "false",
                device.value,
                device.hue,
                device.sat
        );
    }
    else
    {
        snprintf_P(
                buffer, sizeof(buffer),
                FAUXMO_DEVICE_JSON_TEMPLATE_SHORT,
                device.name.c_str(),
                device.uniqueid
        );
    }

    return { buffer };
}

/**
 * Respond to a TCP /description.xml request
 * @param client TCP client
 * @param url URL
 * @param body Body
 * @return True if handled
 */
bool FauxmoESP::_onTCPDescription(AsyncClient* client, [[maybe_unused]] const String& url,
        [[maybe_unused]] const String& body) const
{
    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Handling /description.xml request\n")

    IPAddress ip = WiFi.localIP();
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    mac.toLowerCase();

    char response[strlen_P(FAUXMO_DESCRIPTION_TEMPLATE) + 64];
    snprintf_P(
            response, sizeof(response),
            FAUXMO_DESCRIPTION_TEMPLATE,
            ip[0], ip[1], ip[2], ip[3], _tcp_port,
            ip[0], ip[1], ip[2], ip[3], _tcp_port,
            mac.c_str(), mac.c_str()
    );

    _sendTCPResponse(client, "200 OK", response, "text/xml");
    return true;
}

/**
 * Handle a TCP /api request
 * @param client TCP client
 * @param url URL
 * @param body Body
 * @return True if handled
 */
bool FauxmoESP::_onTCPList(AsyncClient* client, const String& url, [[maybe_unused]] const String& body)
{
    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Handling list request\n")

    // Get the index
    int pos = url.indexOf("lights");
    if (-1 == pos)
    {
        return false;
    }

    // Get the id
    unsigned char id = url.substring(pos + 7).toInt();

    // This will hold the response string
    String response;

    // Client is requesting all devices
    if (0 == id)
    {
        response += "{";
        for (size_t i = 0; i < _devices.size(); i++)
        {
            if (i > 0) response += ",";
            response += "\"" + String(i + 1) + "\":" + _deviceJson(i, false);    // send short template
        }
        response += "}";

        // Client is requesting a single device
    }
    else
    {
        response = _deviceJson(id - 1);
    }

    char responseStr[response.length() + 1];
    response.toCharArray(responseStr, response.length() + 1);

    _sendTCPResponse(client, "200 OK", responseStr, "application/json");

    return true;

}

/**
 * Handle a TCP /api POST request
 * @param client TCP client
 * @param url URL
 * @param body Body
 * @return True if handled
 */
bool FauxmoESP::_onTCPControl(AsyncClient* client, const String& url, const String& body)
{
    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "_onTCPControl: URL: %s, body: %s\n", url.c_str(), body.c_str())

    // "devicetype" request
    if (body.indexOf("devicetype") > 0)
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Handling devicetype request\n")

        _sendTCPResponse(client, "200 OK", R"([{"success":{"username": ")" FAUXMO_DEVICE_USERNAME R"("}}])",
                "application/json");
        return true;
    }

    // "state" request
    if ((url.indexOf("state") > 0) && (body.length() > 0))
    {
        // Get the index
        int pos = url.indexOf("lights");
        if (-1 == pos) return false;

        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Handling state request\n")

        // Get the index
        unsigned char id = url.substring(pos + 7).toInt();
        if (id > 0)
        {
            --id;
            // Brightness
            pos = body.indexOf("bri");
            if (pos > 0)
            {
                unsigned char value = body.substring(pos + 5).toInt();
                _devices[id].value = value;
                _devices[id].state = (value > 0);
            }
            else if (body.indexOf("false") > 0)
            {
                _devices[id].state = false;
            }
            else
            {
                _devices[id].state = true;
                if (0 == _devices[id].value) _devices[id].value = 255;
            }

            // Hue
            pos = body.indexOf("hue");
            if (pos > 0)
            {
                unsigned char hue = body.substring(pos + 5).toInt();
                _devices[id].hue = hue;
            }

            // Saturation
            pos = body.indexOf("sat");
            if (pos > 0)
            {
                unsigned char sat = body.substring(pos + 5).toInt();
                _devices[id].sat = sat;
            }

            char response[strlen_P(FAUXMO_TCP_STATE_RESPONSE) + 10];
            snprintf_P(
                    response, sizeof(response),
                    FAUXMO_TCP_STATE_RESPONSE,
                    id + 1, _devices[id].state ? "true" : "false", id + 1,
                    _devices[id].value, id + 1, _devices[id].hue, id + 1, _devices[id].sat
            );
            _sendTCPResponse(client, "200 OK", response, "text/xml");

            if (_setCallback)
            {
                _setCallback(id, &_devices[id]);
            }
            return true;
        }
    }
    return false;
}

/**
 * Handle a TCP request
 * @param client TCP client
 * @param isGet True if GET request, false if POST
 * @param url URL
 * @param body Body
 * @return True if handled
 */
bool FauxmoESP::_onTCPRequest(AsyncClient* client, bool isGet, const String& url, const String& body)
{
    if (!_fauxmoEnabled)
    {
        return false;
    }

    #if DEBUG_FAUXMO_VERBOSE_TCP
    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "isGet: %s\n", isGet ? "true" : "false");
    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "URL: %s\n", url.c_str());
    if (!isGet)
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Body:\n%s\n", body.c_str());
    }
    #endif

    if (url.equals("/description.xml"))
    {
        return _onTCPDescription(client, url, body);
    }

    if (url.startsWith("/api"))
    {
        if (isGet)
        {
            return _onTCPList(client, url, body);
        }
        else
        {
            return _onTCPControl(client, url, body);
        }
    }

    if (_redirect_port != 0)
    {
        char response[strlen_P(FAUXMO_REDIRECT) + 5];
        snprintf_P(
                response, sizeof(response),
                FAUXMO_REDIRECT,
                _redirect_port
        );
        _sendTCPResponse(client, "200 OK", response, "text/html");
    }

    return false;
}

bool FauxmoESP::_onTCPData(AsyncClient* client, void* data, size_t len)
{
    if (!_fauxmoEnabled)
    {
        return false;
    }

    char* p = static_cast<char*>(data);
    p[len] = 0;

    #if DEBUG_FAUXMO_VERBOSE_TCP
    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "TCP request\n%s\n", p);
    #endif

    // Method is the first word of the request
    char* method = p;

    while (*p != ' ') p++;
    *p = 0;
    p++;

    // Split word and flag start of url
    char* url = p;

    // Find next space
    while (*p != ' ') p++;
    *p = 0;
    p++;

    // Find double line feed
    unsigned char c = 0;
    while ((*p != 0) && (c < 2))
    {
        if (*p != '\r')
        {
            c = (*p == '\n') ? c + 1 : 0;
        }
        p++;
    }
    char* body = p;

    bool isGet = (strncmp(method, "GET", 3) == 0);

    return _onTCPRequest(client, isGet, url, body);
}

[[maybe_unused]] void FauxmoESP::_onUDPData(const IPAddress& remoteIP, unsigned int remotePort, void* data, size_t len)
{

}

void FauxmoESP::_onTCPClient(AsyncClient* client)
{
    if (_fauxmoEnabled)
    {
        [[maybe_unused]] unsigned int i = 0;
        for (auto& _tcpClient : _tcpClients)
        {
            if (!_tcpClient || !_tcpClient->connected())
            {
                _tcpClient = client;
                client->onAck([](void* s, AsyncClient* c, size_t len, uint32_t time)
                {
                }, nullptr);

                client->onData([this](void* s, AsyncClient* c, void* data, size_t len)
                {
                    _onTCPData(c, data, len);
                }, nullptr);

                client->onDisconnect([i, &_tcpClient](void* s, AsyncClient* c)
                {
                    if (_tcpClient != nullptr)
                    {
                        _tcpClient->free();
                        _tcpClient = nullptr;
                    }
                    else
                    {
                        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Client %d already disconnected\n", i)
                    }
                    delete c;
                    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Client #%d disconnected\n", i)
                }, nullptr);

                client->onError([i](void* s, AsyncClient* c, int8_t error)
                {
                    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Error %s (%d) on client #%d\n", c->errorToString(error), error, i)
                }, nullptr);

                client->onTimeout([i](void* s, AsyncClient* c, uint32_t time)
                {
                    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Timeout on client #%d at %i\n", i, time)
                    c->close();
                }, nullptr);

                client->setRxTimeout(FAUXMO_RX_TIMEOUT);

                DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Client #%d connected\n", i)
                return;
            }
            i++;
        }

        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Rejecting - Too many connections\n")
    }
    else
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Rejecting - Disabled\n")
    }

    client->onDisconnect([](void* s, AsyncClient* c)
    {
        c->free();
        delete c;
    });
    client->close(true);
}

[[maybe_unused]] void FauxmoESP::setDeviceUniqueId(unsigned char id, const char* uniqueId)
{
    strncpy(_devices[id].uniqueid, uniqueId, FAUXMO_DEVICE_UNIQUE_ID_LENGTH);
}

[[maybe_unused]] unsigned char FauxmoESP::addDevice(const char* deviceName)
{
    fauxmoesp_device_t device;
    unsigned char deviceId = _devices.size();

    // init properties
    device.name = strdup(deviceName);
    device.state = false;
    device.value = 0;
    device.hue = 0;
    device.sat = 0;

    // create the uniqueid
    String mac = WiFi.macAddress();

    snprintf(device.uniqueid, FAUXMO_DEVICE_UNIQUE_ID_LENGTH, "%s:%s-%02X", mac.c_str(), "00:00", deviceId);

    // Attach
    _devices.push_back(device);

    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Device '%s' added as #%d\n", deviceName, deviceId)

    return deviceId;
}

unsigned int FauxmoESP::getDeviceId(const String& deviceName)
{
    for (size_t id = 0; id < _devices.size(); id++)
    {
        if (deviceName == _devices[id].name)
        {
            return id;
        }
    }

    return FAUXMO_MAX_DEVICES;
}

bool FauxmoESP::renameDevice(unsigned char id, const String& newDeviceName)
{
    if (id >= _devices.size())
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Device #%d not found\n", id)
        return false;
    }

    //_devices[id].name = strdup(newDeviceName);
    _devices[id].name = String(newDeviceName.c_str());
    _devices[id].name.toUpperCase();

    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Device #%d renamed to '%s'\n", id, newDeviceName.c_str())
    return true;
}

[[maybe_unused]] bool FauxmoESP::renameDevice(const char* oldDeviceName, const char* newDeviceName)
{
    unsigned char id = getDeviceId(oldDeviceName);
    return renameDevice(id, newDeviceName);
}

bool FauxmoESP::removeDevice(unsigned char id)
{
    if (id >= _devices.size())
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Device #%d not found\n", id)
        return false;
    }

    _devices.erase(_devices.begin() + id);
    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Device #%d removed\n", id)

    return true;
}

[[maybe_unused]] bool FauxmoESP::removeDevice(const char* deviceName)
{
    unsigned int id = getDeviceId(deviceName);
    return removeDevice(id);
}

[[maybe_unused]] String FauxmoESP::getDeviceName(unsigned char id)
{
    if (id >= _devices.size())
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Device #%d not found\n", id)
        return "";
    }

    return {_devices[id].name};
}

bool FauxmoESP::setState(unsigned char id, bool state, unsigned char value, unsigned char hue, unsigned char sat)
{
    if (id >= _devices.size())
    {
        DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "Device #%d not found\n", id)
        return false;
    }

    _devices[id].state = state;
    _devices[id].value = value;
    _devices[id].hue = hue;
    _devices[id].sat = sat;
    return true;
}

[[maybe_unused]] bool
FauxmoESP::setState(const char* deviceName, bool state, unsigned char value, unsigned char hue, unsigned char sat)
{
    unsigned char deviceId = getDeviceId(deviceName);
    return setState(deviceId, state, value, hue, sat);
}

[[maybe_unused]] bool FauxmoESP::process(AsyncClient* client, bool isGet, const String& url, const String& body)
{
    return _onTCPRequest(client, isGet, url, body);
}

void FauxmoESP::handle()
{
    if (!_fauxmoEnabled)
    {
        return;
    }

    _handleUDP();
}

[[maybe_unused]] void FauxmoESP::enable(bool enable)
{
    if (enable == _fauxmoEnabled)
    {
        return;
    }

    _fauxmoEnabled = enable;
    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "%s\n", _fauxmoEnabled ? "Enabled" : "Disabled")

    if (!_fauxmoEnabled)
    {
        return;
    }

    // Start TCP server if internal
    if (_internalWebServerEnabled)
    {
        if (_server == nullptr)
        {
            _server = new AsyncServer(_tcp_port);
            _server->onClient([this](void* s, AsyncClient* c)
            {
                _onTCPClient(c);
            }, nullptr);
        }
        _server->begin();
    }

    // UDP setup
    #ifdef ESP32
    _udp.beginMulticast(FAUXMO_UDP_MULTICAST_IP, FAUXMO_UDP_MULTICAST_PORT);
    #else
    _udp.beginMulticast(WiFi.localIP(), FAUXMO_UDP_MULTICAST_IP, FAUXMO_UDP_MULTICAST_PORT);
    #endif
    DEBUG_MSG_FAUXMO(FAUXMO_LOG_TAG "UDP server started\n")
}
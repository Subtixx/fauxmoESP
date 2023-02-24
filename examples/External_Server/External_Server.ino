#include <Arduino.h>
#if defined(ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESP8266WebServer.h>
#elif defined(ESP32)
    #include <WiFi.h>
    #include <WebServer.h>
#endif
#include "FauxmoESP.h"

#define WIFI_SSID "your_ssid"
#define WIFI_PASS "your_password"

FauxmoESP fauxmo;
#if defined(ESP8266)
    ESP8266WebServer* server = new ESP8266WebServer(80);
#elif defined(ESP32)
    WebServer* server = new WebServer(80);
#endif

// -----------------------------------------------------------------------------

#define SERIAL_BAUDRATE                 115200
#define LED                             2

// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------

void wifiSetup()
{

    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(),
            WiFi.localIP().toString().c_str());

}

void serverSetup()
{

    // Custom entry point (not required by the library, here just as an example)
    server->on(Uri("/index.html"), HTTP_GET, []
    {
        server->send(200, "text/html", "<h1>Hello World!</h1>");
    });

    // These two callbacks are required for gen1 and gen3 compatibility
    server->on(UriRegex("/api/(.*)"), HTTP_ANY, []
    {
        String response;
        uint16_t code = 0;

        String method = server->method() == HTTP_GET ? "GET" : server->method() == HTTP_POST ? "POST" : server->method() == HTTP_PUT ? "PUT" : server->method() == HTTP_DELETE ? "DELETE" : "UNKNOWN";
        fauxmo.processRequest(server->uri(), server->arg("body"), method, response, &code);
        // Handle any other body request here...
    });
    server->onNotFound([]
    {
        server->send(404, "text/plain", "Not found");
    });

    // Start the server
    server->begin();

}

void setup()
{

    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println();

    // LED
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH); // Our LED has inverse logic (high for OFF, low for ON)

    // Wifi
    wifiSetup();

    // Web server
    serverSetup();

    // You can use different ways to invoke alexa to modify the devices state:
    // "Alexa, turn kitchen on" ("kitchen" is the name of the first device below)
    // "Alexa, turn on kitchen"
    // "Alexa, set kitchen to fifty" (50 means 50% of brightness)

    // Add virtual devices
    fauxmo.addLight("kitchen");
    fauxmo.addLight("livingroom");

    // You can add more devices
    //fauxmo.addLight("light 3");
    //fauxmo.addLight("light 4");
    //fauxmo.addLight("light 5");
    //fauxmo.addLight("light 6");
    //fauxmo.addLight("light 7");
    //fauxmo.addLight("light 8");
    fauxmo.setup([](Light* light, LightStateChange* change)
    {
        // Callback when a command from Alexa is received. 
        // You can use device_id or device_name to choose the element to perform an action onto (relay, LED,...)
        // State is a boolean (ON/OFF), value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here),
        // hue a number from 0 to 255, and sat a number from 0 to 255. You will be responsible for converting hue, saturation, and value 
        // (HSV) to RGB. Here is a resource to get you started:
        // https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both  
        // Just remember not to delay too much here, this is a callback, exit as soon as possible.
        // If you have to do something more involved here set a flag and process it in your main loop.

        // if (0 == device_id) digitalWrite(RELAY1_PIN, state);
        // if (1 == device_id) digitalWrite(RELAY2_PIN, state);
        // if (2 == device_id) analogWrite(LED1_PIN, value);

        Serial.printf("[MAIN] Device #%d (%s) state: %s brightness: %d | hue: %d | saturation: %d \n",
                light->deviceId, light->name.c_str(), light->state.isOn ? "ON" : "OFF", light->state.brightness,
                light->state.hue, light->state.saturation);

        // For the example we are turning the same LED on and off regardless fo the device triggered or the value
        digitalWrite(LED, !light->state.isOn); // we are nor-ing the state because our LED has inverse logic.

    }, [](Light* light)
    {

    });

}

void loop()
{
    // This is a sample code to output free heap every 5 seconds
    // This is a cheap way to detect memory leaks
    static unsigned long last = millis();
    if (millis() - last > 5000)
    {
        last = millis();
        Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
    }
}

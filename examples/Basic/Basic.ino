#include <Arduino.h>

#ifdef ESP32
    #include <WiFi.h>
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
#endif
#include "FauxmoESP.h"

FauxmoESP fauxmo;

#define WIFI_SSID "your_ssid"
#define WIFI_PASS "your_password"

// -----------------------------------------------------------------------------

#define SERIAL_BAUDRATE     115200

#define LED_YELLOW          4
#define LED_GREEN           5
#define LED_BLUE            0
#define LED_PINK            2
#define LED_WHITE           15

#define ID_YELLOW           "yellow lamp"
#define ID_GREEN            "green lamp"
#define ID_BLUE             "blue lamp"
#define ID_PINK             "pink lamp"
#define ID_WHITE            "white lamp"

// -----------------------------------------------------------------------------

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

void setup()
{

    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println();

    // LEDs
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_PINK, OUTPUT);
    pinMode(LED_WHITE, OUTPUT);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_PINK, LOW);
    digitalWrite(LED_WHITE, LOW);

    // Wifi
    wifiSetup();

    // By default, fauxmoESP creates it's own webserver on the defined port
    // The TCP port must be 80 for gen3 devices (default is 1901)
    // This has to be done before the call to enable()
    fauxmo.setWebServerEnabled(true); // not needed, this is the default value
    fauxmo.setWebServerPort(80); // This is required for gen3 devices

    // You have to call enable(true) once you have a WiFi connection
    // You can enable or disable the library at any moment
    // Disabling it will prevent the devices from being discovered and switched
    fauxmo.setup([](Light* light, LightStateChange* change)
    {
        // Callback when a command from Alexa is received.
        // You can use device_id or device->name to choose the element to perform an action onto (relay, LED,...)
        // State is a boolean (ON/OFF), value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here),
        // hue a number from 0 to 255, and sat a number from 0 to 255. You will be responsible for converting hue, saturation, and value
        // (HSV) to RGB. Here is a resource to get you started:
        // https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
        // Just remember not to delay too much here, this is a callback, exit as soon as possible.
        // If you have to do something more involved here set a flag and process it in your main loop.


        Serial.printf("[MAIN] Light #%d (%s/%s) state: %s brightness: %d | hue: %d | saturation: %d \n",
                light->deviceId, light->uniqueId, light->name.c_str(), light->state.isOn ? "ON" : "OFF", light->state.brightness,
                light->state.hue, light->state.saturation);
        if (light->name == ID_YELLOW)
        {
            if (change->isOnSet())
            {
                digitalWrite(LED_YELLOW, change->getIsOn() ? HIGH : LOW);
                change->setOnSuccess(true);
            }
        }
        else if (light->name == ID_GREEN)
        {
            if (change->isOnSet())
            {
                digitalWrite(LED_GREEN, change->getIsOn() ? HIGH : LOW);
                change->setOnSuccess(true);
            }
        }
        else if (light->name == ID_BLUE)
        {
            if (change->isOnSet())
            {
                digitalWrite(LED_BLUE, change->getIsOn() ? HIGH : LOW);
                change->setOnSuccess(true);
            }
        }
        else if (light->name == ID_PINK)
        {
            if (change->isOnSet())
            {
                digitalWrite(LED_PINK, change->getIsOn() ? HIGH : LOW);
                change->setOnSuccess(true);
            }
        }
        else if (light->name == ID_WHITE)
        {
            if (change->isOnSet())
            {
                digitalWrite(LED_WHITE, change->getIsOn() ? HIGH : LOW);
                change->setOnSuccess(true);
            }
        }
    }, [](Light* light)
    {
        Serial.printf("[MAIN] Light #%d (%s/%s) state requested\n", light->deviceId, light->uniqueId, light->name.c_str());
        if (light->name == ID_YELLOW)
        {
            light->state.isOn = digitalRead(LED_YELLOW) == HIGH;
        }
        else if (light->name == ID_GREEN)
        {
            light->state.isOn = digitalRead(LED_GREEN) == HIGH;
        }
        else if (light->name == ID_BLUE)
        {
            light->state.isOn = digitalRead(LED_BLUE) == HIGH;
        }
        else if (light->name == ID_PINK)
        {
            light->state.isOn = digitalRead(LED_PINK) == HIGH;
        }
        else if (light->name == ID_WHITE)
        {
            light->state.isOn = digitalRead(LED_WHITE) == HIGH;
        }
    });

    // You can use different ways to invoke alexa to modify the devices state:
    // "Alexa, turn yellow lamp on"
    // "Alexa, turn on yellow lamp
    // "Alexa, set yellow lamp to fifty" (50 means 50% of brightness, note, this example does not use this functionality)

    // Add virtual devices
    fauxmo.addLight(ID_YELLOW);
    fauxmo.addLight(ID_GREEN);
    fauxmo.addLight(ID_BLUE);
    fauxmo.addLight(ID_PINK);
    fauxmo.addLight(ID_WHITE);
}

void loop()
{
    // fauxmoESP uses an async TCP server but a sync UDP server
    // Therefore, we have to manually poll for UDP packets
    fauxmo.update();

    // This is a sample code to output free heap every 5 seconds
    // This is a cheap way to detect memory leaks
    static unsigned long last = millis();
    if (millis() - last > 5000)
    {
        last = millis();
        Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
    }

    // If your device state is changed by any other means (MQTT, physical button,...)
    // you can instruct the library to report the new state to Alexa on next request:
    // fauxmo.setState(ID_YELLOW, true, 255);
}

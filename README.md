<center>
    <img alt="Logo" src="./docs/include/logo.svg" height="256" />
</center>

Amazon Alexa support for ESP8266, ESP32 and Raspberry Pi Pico W devices.

This is a library for ESP8266/ESP32-based/Raspberry Pi Pico W devices that emulates Philips Hue lights and thus allows you to control them using this protocol, in particular from Alexa-powered devices like the Amazon Echo or the Dot.

This is a fork of FauxmoESP with some changes! For more see [CHANGELOG.md](CHANGELOG.md)

<center>

![GitHub Workflow Status](https://img.shields.io/github/actions/workflow/status/subtixx/fauxmoesp/build.yml?color=%2350fa7b&logo=github&logoColor=%23f8f8f2&style=flat-square)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/subtixx/library/FauxmoESP.svg)](https://registry.platformio.org/libraries/subtixx/FauxmoESP)
![GitHub stars](https://img.shields.io/github/stars/subtixx/fauxmoesp.svg?color=ff79c6&logo=github&style=flat-square)
![GitHub forks](https://img.shields.io/github/forks/subtixx/fauxmoesp.svg?color=ff79c6&logo=github&style=flat-square)
[![version](https://img.shields.io/badge/version-1.1.0-brightgreen.svg?style=flat-square&color=ff79c6)](./CHANGELOG.md)
[![license](https://img.shields.io/badge/license-MIT-orange.svg?style=flat-square&color=ffb86c)](./LICENSE)
[![GitHub issues](https://img.shields.io/github/issues/subtixx/fauxmoesp.svg?color=ff79c6&logo=github&style=flat-square)](https://github.com/subtixx/fauxmoesp/issues)
[![GitHub Sponsor](https://img.shields.io/badge/GitHub-%E2%9D%A4%EF%B8%8F%20Sponsor-%23886EB4?logo=github&style=flat-square)](https://github.com/sponsors/Subtixx)

</center>

## Dependencies

Besides the libraries already included with the Arduino Core for ESP8266 or ESP32, these libraries are also required to use fauxmoESP:

ESP8266:

* This library uses [ESPAsyncTCP][3] library by [ottowinter][5]

ESP32:

* This library uses [AsyncTCP][4] library by [esphome][5]

Raspberry Pi Pico

* This library uses [AsyncTCP_RP2040W][8] library by [khoih-prog][9]

### PlatformIO

If you are using PlatformIO (check the section bellow on how to compile it) the required libraries should be installed automatically.

### Arduino IDE

You will need to install the required library from sources. Your best option is to download the library as a ZIP file and install it using the option under "Sketch > Include Library > Add .ZIP Library...".

You can look for it manually but I have gathered the URL here for convenience:

|Device|Library|Repository|ZIP|
|-|-|-|-|
|ESP8266|**ESPAsyncTCP** by Hristo Gochkov ESP8266|[GIT](https://github.com/ottowinter/ESPAsyncTCP)|[ZIP](https://github.com/me-no-dev/ESPAsyncTCP/archive/master.zip)|
|ESP32|**AsyncTCP** by Hristo Gochkov ESP32|[GIT](https://github.com/esphome/AsyncTCP)|[ZIP](https://github.com/me-no-dev/AsyncTCP/archive/master.zip)|
|Raspberry Pi Pico W|**AsyncTCP_RP2040W** by Khoi Hoang |[GIT](https://github.com/khoih-prog/AsyncTCP_RP2040W)|[ZIP](https://github.com/khoih-prog/AsyncTCP_RP2040W/archive/master.zip)|

## Usage

The library is very easy to use, basically instantiate an object, connect to the Wifi, add one or more virtual devices and bind the callback to get the messages. An schematic example could be:

```
#include <FauxmoESP.h>

FauxmoESP fauxmo;

void setup() {

    Serial.begin(115200);

    ... connect to wifi ...

    fauxmo.addDevice("light one");
    fauxmo.addDevice("light two");
    fauxmo.addDevice("light three");
    fauxmo.addDevice("light four");

    fauxmo.setWebServerPort(80); // required for gen3 devices
    fauxmo.enable(true);

    fauxmo.onSetState([](unsigned char deviceId, fauxmoesp_device_t* device) {
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", deviceId, device->name, device->state ? "ON" : "OFF", device->value);
    });

}

void loop() {
    fauxmo.handle();
}

```

(Check the examples folder)

## To use with ESP-IDF

Add `#include "Arduino.h"`


Then run the "discover devices" option from your Alexa app or web (in the Smart Home section). A new device with the name you have configured should appear. Tell Alexa to switch it on or off and check your terminal ;)

## Troubleshooting

Current status of the library:

| Platform                       | Gen1   | Gen2    | Gen3      |
|--------------------------------|--------|---------|-----------|
| ESP8266 Core 2.3.X             | OK     | OK?     | OK (1)    |
| ESP8266 Core 2.4.0             | OK (2) | OK? (2) | OK (1, 2) |
| ESP8266 Core 2.4.1             | OK (2) | OK? (2) | OK (1, 2) |
| ESP8266 Core 2.4.2             | OK (2) | OK? (2) | OK (1, 2) |
| ESP32                          | OK     | OK?     | OK (1)    |
| Raspberry Pi Pico W Core 2.6.0 | OK     | OK      | OK (1)    |

(1) When using gen3 devices TCP port must be 80 always.
(2) Arduino Core for ESP8266 requires LwIP set to "v1.4 Higher Bandwidth".

* fauxmoESP 3.1.X: When using with gen3 devices TCP port must be 80. You can define it with the `setWebServerPort` method.

* fauxmoESP 3.1.X: If you application already uses port 80 you can prevent fauxmoESP from creating its own webserver and inject the values from your application handlers to the library. Check the fauxmoESP_External_Server example.

* fauxmoESP 3.X.X: When using Arduino Core for ESP8266 v2.4.X, double check you are building the project with LwIP Variant set to "v1.4 Higher Bandwidth". You can change it from the Tools menu in the Arduino IDE or passing the `-DPIO_FRAMEWORK_ARDUINO_LWIP_HIGHER_BANDWIDTH` build flag to PlatformIO.

* fauxmoESP 2.X.X: The `onGetState` method accepts a function (a callback) that will be called when a new message arrives. Try not to do many things inside the callback, it should return as fast as possible. Instead of adding logic here just save the data (device_id and state, for instance) and process it from your main loop.

* Some people have reported problems when the ESP and the Alexa devices are connected to different wireless networks (like 2.4 and 5GHz bands on some routers). See https://bitbucket.org/xoseperez/fauxmoesp/issues/53.

* Latest version of ESP Async Webserver fails building with the current setup. Use version 1.2.2 max. See fauxmoESP_External_Server example `platformio.ini` file.

[1]:https://github.com/esp8266/Arduino
[2]:http://docs.platformio.org/en/stable/platforms/espressif8266.html#using-arduino-framework-with-staging-version
[3]:https://github.com/me-no-dev/ESPAsyncTCP
[4]:https://github.com/me-no-dev/AsyncTCP
[5]:https://github.com/me-no-dev
[6]:https://github.com/makermusings/fauxmo
[7]:https://github.com/earlephilhower/arduino-pico
[8]:https://github.com/khoih-prog/AsyncTCP_RP2040W
[9]:https://github.com/khoih-prog

## License

Copyright (C) 2023 by Dominic J. S. Hock <d.hock@it-hock.de>
Copyright (C) 2016-2020 by Xose Pérez <xose dot perez at gmail dot com>, 2020 by Paul Vint <pjvint at gmail dot com>

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

# 🔥 **Subtixx/FauxmoESP**

<div align="center">
<a href="https://github.com/Subtixx/FauxmoESP">
    <img width="100%" src="https://github.com/Subtixx/fauxmoESP/raw/main/docs/include/logo.svg" alt="@Subtixx/FauxmoESP's logo">
</a>
<img src="https://img.shields.io/github/stars/subtixx/fauxmoesp.svg?color=ff79c6&logo=github&style=for-the-badge"/>
<img src="https://img.shields.io/github/forks/subtixx/fauxmoesp.svg?color=ff79c6&logo=github&style=for-the-badge"/>
<br>
<a href="https://github.com/Subtixx/FauxmoESP/issues">
    <img src="https://img.shields.io/github/issues/Subtixx/FauxmoESP?color=ff79c6&style=for-the-badge&logo=github" alt="Subtixx/FauxmoESP's issues"/>
</a>
<a href="https://github.com/Subtixx/FauxmoESP/pulls">
    <img src="https://img.shields.io/github/issues-pr/Subtixx/FauxmoESP?color=ff79c6&style=for-the-badge&logo=github" alt="Subtixx/FauxmoESP's pull requests"/>
</a>
<img src="https://img.shields.io/github/actions/workflow/status/subtixx/fauxmoesp/build.yml?color=%2350fa7b&logo=github&logoColor=%23f8f8f2&style=for-the-badge"/>

<a href="https://github.com/Subtixx/FauxmoESP/raw/main/LICENSE">
    <img src="https://img.shields.io/badge/license-MIT-orange.svg?style=for-the-badge&color=ffb86c">
</a>

<br/>

<a href="https://registry.platformio.org/libraries/subtixx/FauxmoESP">
    <img src="https://badges.registry.platformio.org/packages/subtixx/library/FauxmoESP.svg">
</a>

</div>

---

## 🤔 **About the project**

Amazon Alexa support for ESP8266, ESP32 and Raspberry Pi Pico W devices.

This is a library for ESP8266/ESP32-based/Raspberry Pi Pico W devices that emulates Philips Hue lights and thus allows you to control them using this protocol, in particular from Alexa-powered devices like the Amazon Echo or the Dot.

This is a fork of FauxmoESP with some changes! For more see [CHANGELOG.md](https://github.com/Subtixx/fauxmoESP/blob/main/CHANGELOG.md)

---

## ⚡ **Installation**

### PlatformIO

> pio lib install "Subtixx/FauxmoESP"

or

> lib_deps = Subtixx/FauxmoESP

---

## 🚀 **Usage**

The library is very easy to use, basically instantiate an object, connect to the Wifi, add one or more virtual devices and bind the callback to get the messages. An schematic example could be:

```
#include <FauxmoESP.h>

FauxmoESP fauxmo;

void setup() {

    Serial.begin(115200);

    ... connect to wifi ...

    fauxmo.addLight("light one");
    fauxmo.addLight("light two");
    fauxmo.addLight("light three");
    fauxmo.addLight("light four");

    fauxmo.setWebServerPort(80); // required for gen3 devices
    fauxmo.setWebServerEnabled(true);

    fauxmo.setup([](Light* light) {
        Serial.printf("Device %s state: %s\n", light->name.c_str(), light->state ? "ON" : "OFF");
    }, [](Light* light) {
    });

}

void loop() {
    fauxmo.handle();
}

```

(Check the examples folder)

## To use with ESP-IDF

Add `#include "Arduino.h"`

---

## 🌲 **Project tree**

<!-- ... [SHOW YOUR PROJECT TREE HERE IF USEFUL] -->

---

## 📝 **Additional notes**

Besides the libraries already included with the Arduino Core for ESP8266 or ESP32, 
these libraries are also required to use fauxmoESP:

ESP8266:

* This library uses [ESPAsyncTCP][3] library by [ottowinter][5]

ESP32:

* This library uses [AsyncTCP][4] library by [esphome][6]

Raspberry Pi Pico

* This library uses [AsyncTCP_RP2040W][8] library by [khoih-prog][9]


---

## 🍰 **Supporters and donators**

<a href="https://github.com/Subtixx/FauxmoESP">
  <img alt="@Subtixx/FauxmoESP's brand logo without text" align="right" src="https://github.com/Subtixx/fauxmoESP/raw/main/docs/include/logo.svg" width="18%" />
</a>


We are currently looking for new donators to help and maintain this project! ❤️

By donating, you will help the development of this project, and *you will be featured in this FauxmoESP's README.md*, so
everyone can see your kindness and visit your content ⭐.

<a href="https://github.com/sponsors/Subtixx">
  <img src="https://img.shields.io/badge/Sponsor-Subtixx/FauxmoESP-blue?logo=github-sponsors&style=for-the-badge&color=red">
</a>

---

FauxmoESP was generated from *[Josee9988/project-template](https://github.com/Josee9988/project-template)* 📚

---

## 🎉 Was the API helpful? Help us raise these numbers up

[![GitHub followers](https://img.shields.io/github/followers/Subtixx.svg?style=social)](https://github.com/Subtixx)
[![GitHub stars](https://img.shields.io/github/stars/Subtixx/FauxmoESP.svg?style=social)](https://github.com/Subtixx/FauxmoESP/stargazers)
[![GitHub watchers](https://img.shields.io/github/watchers/Subtixx/FauxmoESP.svg?style=social)](https://github.com/Subtixx/FauxmoESP/watchers)
[![GitHub forks](https://img.shields.io/github/forks/Subtixx/FauxmoESP.svg?style=social)](https://github.com/Subtixx/FauxmoESP/network/members)
<!-- MODIFY THIS LINK TO YOUR MAIN DONATING SITE IF YOU ARE NOT IN THE GITHUB SPONSORS PROGRAM -->
[![Sponsor](https://img.shields.io/static/v1?label=Sponsor&message=%E2%9D%A4&logo=github-sponsors&color=red&style=social)](https://github.com/sponsors/Subtixx)

Enjoy! 😃

---

## ⚖️📝 **License and Changelog**

See the license in the '**[LICENSE](LICENSE)**' file.

Watch the changes in the '**[CHANGELOG.md](CHANGELOG.md)**' file.

---

_Made with a lot of ❤️❤️ by **[@Subtixx](https://github.com/Subtixx)**_

[5]:https://github.com/ottowinter
[6]:https://github.com/esphome
[9]:https://github.com/khoih-prog
[3]:https://github.com/ottowinter/ESPAsyncTCP
[4]:https://github.com/esphome/AsyncTCP
[7]:https://github.com/earlephilhower/arduino-pico
[8]:https://github.com/khoih-prog/AsyncTCP_RP2040W

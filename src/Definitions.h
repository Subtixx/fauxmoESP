#pragma once

#include <functional>
#include "LightState.h"

#define FAUXMO_LOG_TAG            "[FAUXMO] "
#define DEVICE_UNIQUE_ID_LENGTH 27

#ifdef DEBUG_FAUXMO
    #if defined(ARDUINO_ARCH_ESP32)
        #define DEBUG_MSG_FAUXMO(fmt, ...) { DEBUG_FAUXMO.printf_P((PGM_P) PSTR(fmt), ## __VA_ARGS__); }
    #else
        #define DEBUG_MSG_FAUXMO(fmt, ...) { DEBUG_FAUXMO.printf(fmt, ## __VA_ARGS__); }
    #endif
#else
    #define DEBUG_MSG_FAUXMO(...) {}
#endif

typedef std::function<void(LightState)> OnStateChangeCallback;
typedef std::function<bool()> OnGetStateCallback;

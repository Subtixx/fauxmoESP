#pragma once

#include <functional>

struct LightState;
struct Light;

#define FAUXMO_LOG_TAG            "[FAUXMO] "
#define DEVICE_UNIQUE_ID_LENGTH 27
#define MAX_DEVICES 65535

#if defined(DEBUG)
    #define DEBUG_FAUXMO Serial
#endif

#ifdef DEBUG_FAUXMO
    #if defined(ARDUINO_ARCH_ESP32)
        #define DEBUG_MSG_FAUXMO(fmt, ...) { DEBUG_FAUXMO.printf_P((PGM_P) PSTR(fmt), ## __VA_ARGS__); }
    #else
        #define DEBUG_MSG_FAUXMO(fmt, ...) { DEBUG_FAUXMO.printf(fmt, ## __VA_ARGS__); }
    #endif
#else
    #define DEBUG_MSG_FAUXMO(...) {}
#endif

/**
 * @brief Callback function type that is called when a light should change its state
 *
 * @param light The light that should change its state
 */
typedef std::function<void(Light*)> OnStateChangeCallback;

/**
 * @brief Callback function type that is called when a light should return its state
 * Modify the state of the light in this callback function to return the current state
 *
 * @param light The light that was requested
 */
typedef std::function<void(Light*)> OnGetStateCallback;

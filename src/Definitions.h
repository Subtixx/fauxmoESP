#pragma once

#include <functional>
#include "LightStateChange.h"

struct LightState;
struct Light;

#define FAUXMO_VERSION "2.0.0"
#define FAUXMO_LOG_TAG            "[FAUXMO] "
#define DEVICE_UNIQUE_ID_LENGTH 27
#define MAX_DEVICES 65535

constexpr unsigned int compileYear =
        (__DATE__[7] - '0') * 1000 + (__DATE__[8] - '0') * 100 + (__DATE__[9] - '0') * 10 + (__DATE__[10] - '0');
constexpr unsigned int compileMonth = (__DATE__[0] == 'J') ? ((__DATE__[1] == 'a') ? 1 : ((__DATE__[2] == 'n') ? 6
                                                                                                               : 7))    // Jan, Jun or Jul
                                                           : (__DATE__[0] == 'F')
                                                             ? 2                                                              // Feb
                                                             : (__DATE__[0] == 'M') ? ((__DATE__[2] == 'r') ? 3
                                                                                                            : 5)                                 // Mar or May
                                                                                    : (__DATE__[0] == 'A')
                                                                                      ? ((__DATE__[1] == 'p') ? 4
                                                                                                              : 8)                                 // Apr or Aug
                                                                                      : (__DATE__[0] == 'S')
                                                                                        ? 9                                                              // Sep
                                                                                        : (__DATE__[0] == 'O')
                                                                                          ? 10                                                             // Oct
                                                                                          : (__DATE__[0] == 'N')
                                                                                            ? 11                                                             // Nov
                                                                                            : (__DATE__[0] == 'D')
                                                                                              ? 12                                                             // Dec
                                                                                              : 0;
constexpr unsigned int compileDay = (__DATE__[4] == ' ') ? (__DATE__[5] - '0') : (__DATE__[4] - '0') * 10
        + (__DATE__[5] - '0');

constexpr char IsoDate[] =
        { compileYear / 1000 + '0', (compileYear % 1000) / 100 + '0', (compileYear % 100) / 10 + '0',
          compileYear % 10 + '0',
          '-', compileMonth / 10 + '0', compileMonth % 10 + '0',
          '-', compileDay / 10 + '0', compileDay % 10 + '0',
          0
        };

constexpr unsigned int compileHour = (__TIME__[0] - '0') * 10 + (__TIME__[1] - '0');
constexpr unsigned int compileMinute = (__TIME__[3] - '0') * 10 + (__TIME__[4] - '0');
constexpr unsigned int compileSecond = (__TIME__[6] - '0') * 10 + (__TIME__[7] - '0');

constexpr char IsoTime[] =
        { compileHour / 10 + '0', compileHour % 10 + '0',
          ':', compileMinute / 10 + '0', compileMinute % 10 + '0',
          ':', compileSecond / 10 + '0', compileSecond % 10 + '0',
          0
        };

#define DEBUG 1

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
typedef std::function<void(Light*, LightStateChange*)> OnStateChangeCallback;

/**
 * @brief Callback function type that is called when a light should return its state
 * Modify the state of the light in this callback function to return the current state
 *
 * @param light The light that was requested
 */
typedef std::function<void(Light*)> OnGetStateCallback;

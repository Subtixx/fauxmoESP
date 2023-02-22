#pragma once

#include <Arduino.h>

#include "Definitions.h"
#include "LightState.h"
#include "LightCapabilities.h"

struct Light
{
    String name;
    /**
     * Unique id of the device.
     * The MAC address of the device with a unique endpoint id
     * in the form: AA:BB:CC:DD:EE:FF:00:11-XX
     */
    char uniqueId[DEVICE_UNIQUE_ID_LENGTH];

    LightState state = LightState();
    LightCapabilities capabilities = LightCapabilities();

    [[nodiscard]] String toJson() const;
};

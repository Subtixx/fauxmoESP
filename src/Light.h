#pragma once

#include <Arduino.h>

#include "Definitions.h"
#include "LightState.h"
#include "LightCapabilities.h"

struct Light
{
    Light(const String& deviceName, uint deviceId);

    String name;

    /**
     * Max possible is FF:FF -> 65535 (MAX_DEVICES)
     */
    ushort deviceId;
    /**
     * Unique id of the device.
     * The MAC address of the device with a unique endpoint id
     * in the form: AA:BB:CC:DD:EE:FF:00:11-XX
     */
    char uniqueId[DEVICE_UNIQUE_ID_LENGTH]{};

    LightState state = LightState();
    LightCapabilities capabilities = LightCapabilities();

    String type = "Extended color light";
    String modelid = "LCT015";
    String manufacturername = "FauxmoESP";
    String productname = "E4";
    String swversion = "5.105.0.21169";

    [[nodiscard]] String toJson() const;
};

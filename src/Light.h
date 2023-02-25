#pragma once

#include <Arduino.h>

#include "Definitions.h"
#include "LightState.h"
#include "LightCapabilities.h"

/**
 * @brief Represents a light device that can be controlled by the Hue API.
 */
struct Light
{
    /**
     * @brief Creates a new light device.
     * @param deviceName The name of the device.
     * @param deviceId The id of the device.
     */
    Light(const String& deviceName, uint deviceId);

    /**
     * @brief The name of the device.
     */
    String name;

    /**
     * @brief The id of the device.
     * Max possible is FF:FF -> 65535 (MAX_DEVICES)
     */
    ushort deviceId;

    /**
     * Unique id of the device.
     * The MAC address of the device with a unique endpoint id
     * in the form: AA:BB:CC:DD:EE:FF:00:11-XX
     */
    char uniqueId[DEVICE_UNIQUE_ID_LENGTH]{};

    /**
     * @brief The current state of the device.
     */
    LightState state = LightState();

    /**
     * @brief The capabilities of the device.
     */
    LightCapabilities capabilities = LightCapabilities();

    /**
     * @brief The type of the device.
     * Default: Extended color light
     */
    String type = "Extended color light";

    /**
     * @brief The model id of the device.
     * Default: LCT015
     */
    String modelid = "LCT015";

    /**
     * @brief The manufacturer name of the device.
     * Default: FauxmoESP
     */
    String manufacturername = "FauxmoESP";

    /**
     * @brief The product name of the device.
     * Default: E4
     */
    String productname = "E4";

    /**
     * @brief The software version of the device.
     * Default: 5.105.0.21169
     */
    String swversion = "5.105.0.21169";

    /**
     * @brief Converts the light device to a JSON string.
     * @return The JSON string.
     */
    [[nodiscard]] String toJson() const;
};

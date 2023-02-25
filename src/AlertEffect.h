#pragma once

/**
 * @brief Alert effect is a time limited dynamic effect.
 * Alert effects are used to set the lamp to a time limited dynamic effect i.e.
 * the lamps will change there lamps during a short time period and
 * then return to their previous value. Example is the select breathe cycle.
 * The light or lights do one smooth transition from the current state to a higher brightness
 * in the current color to a lower brightness in the current color and back to the original state.
 */
enum AlertEffect
{
    /**
     * No alert effect.
     */
    ALERT_EFFECT_NONE,

    /**
     * The light is performing one breathe cycle.
     */
    ALERT_EFFECT_SELECT,

    /**
     * The light is performing breathe cycles for 15 seconds or until an "alert": "none" command is received.
     */
    ALERT_EFFECT_LSELECT,
};

/**
 * @brief Converts an AlertEffect to a string.
 * @param alertEffect The AlertEffect to convert.
 * @return The string representation of the AlertEffect.
 */
static const char* AlertEffectToString(AlertEffect alertEffect)
{
    switch (alertEffect)
    {
    default:
    case ALERT_EFFECT_NONE:
        return "none";
    case ALERT_EFFECT_SELECT:
        return "select";
    case ALERT_EFFECT_LSELECT:
        return "lselect";
    }
}
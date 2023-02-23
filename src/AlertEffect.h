#pragma once

enum AlertEffect
{
    ALERT_EFFECT_NONE,
    ALERT_EFFECT_SELECT,
    ALERT_EFFECT_LSELECT,
};

static const char* AlertEffectToString(AlertEffect alertEffect)
{
    switch (alertEffect)
    {
    case ALERT_EFFECT_NONE:
        return "none";
    case ALERT_EFFECT_SELECT:
        return "select";
    case ALERT_EFFECT_LSELECT:
        return "lselect";
    default:
        return "none";
    }
}
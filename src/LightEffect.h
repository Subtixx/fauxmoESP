#pragma once

enum LightEffect
{
    LIGHT_EFFECT_NONE,
    LIGHT_EFFECT_COLORLOOP,
};

static const char* LightEffectToString(LightEffect lightEffect)
{
    switch (lightEffect)
    {
    case LIGHT_EFFECT_NONE:
        return "none";
    case LIGHT_EFFECT_COLORLOOP:
        return "colorloop";
    default:
        return "none";
    }
}
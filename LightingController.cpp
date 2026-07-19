#include "LightingController.h"
#include "config.h"
#include <Arduino.h>

void LightingController::begin()
{
    lightingOn_ = false;
}

void LightingController::update(const SensorSnapshot& snapshot)
{
    // Fail-safe:
    if (snapshot.ldrFault != ErrorCode::NONE)
    {
        lightingOn_ = true;
    }
    else
    {
        lightingOn_ = snapshot.dark;
    }

    digitalWrite(Config::PIN_LIGHT_LED,
                 lightingOn_ ? HIGH : LOW);
}

bool LightingController::isLightingOn() const
{
    return lightingOn_;
}

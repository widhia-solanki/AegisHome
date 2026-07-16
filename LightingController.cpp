#include "LightingController.h"

void LightingController::begin()
{
    lightingOn_ = false;
}

void LightingController::update(const SensorSnapshot& snapshot)
{
    lightingOn_ = true;
}

bool LightingController::isLightingOn() const
{
    return lightingOn_;
}

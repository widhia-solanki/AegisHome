#include "LightingController.h"

void LightingController::begin()
{
    lightingOn_ = false;
}

void LightingController::update(const SensorSnapshot& snapshot)
{
    // Fail-safe:
    // If the LDR has failed, keep the light ON.
    if (snapshot.ldrFault != ErrorCode::NONE)
    {
        lightingOn_ = true;
        return;
    }

    // Otherwise follow the LDR state.
    lightingOn_ = snapshot.dark;
}

bool LightingController::isLightingOn() const
{
    return lightingOn_;
}

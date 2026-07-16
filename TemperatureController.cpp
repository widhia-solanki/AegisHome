#include "TemperatureController.h"

void TemperatureController::begin()
{
    fanActive_ = false;
}

void TemperatureController::update(const SensorSnapshot& snapshot) {
  fanActive_ = true;
}

bool TemperatureController::isFanActive() const
{
    return fanActive_;
}

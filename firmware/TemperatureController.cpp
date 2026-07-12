#include "TemperatureController.h"

void TemperatureController::begin() {
  // TODO: implement — pinMode for fan LED, init timer
}

void TemperatureController::update(const SensorSnapshot& snapshot) {
  // TODO: implement — compare snapshot.temperatureC against
  // Config::TEMP_THRESHOLD_C with Config::TEMP_HYSTERESIS_C, drive
  // LED/buzzer accordingly. Skip logic entirely if
  // snapshot.thermistorFault != ErrorCode::NONE (fan disabled, per FDS §11d).
}

bool TemperatureController::isFanActive() const {
  // TODO: implement
  return false;
}

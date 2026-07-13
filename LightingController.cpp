#include "LightingController.h"

void LightingController::begin() {
  // TODO: implement — pinMode for lighting LED
}

void LightingController::update(const SensorSnapshot& snapshot) {
  // TODO: implement — drive LED from snapshot.dark; on
  // snapshot.ldrFault != ErrorCode::NONE, fail to "on" per FDS §11d
}

bool LightingController::isLightingOn() const {
  // TODO: implement
  return false;
}

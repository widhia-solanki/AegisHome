#include "DoorController.h"

void DoorController::begin() {
  // TODO: implement — attach servo, move to Config::SERVO_ANGLE_CLOSED,
  // run boot-time confirmatory sweep (0 -> 10 -> 0) per FDS §11b
}

void DoorController::update(const SensorSnapshot& snapshot) {
  // TODO: implement — on snapshot.doorbellPressed, open + flash welcome
  // LED, start dwell timer; on dwell timer expiry, return to closed
}

bool DoorController::isOpen() const {
  // TODO: implement
  return false;
}

ErrorCode DoorController::getServoFault() const {
  // TODO: implement
  return ErrorCode::NONE;
}

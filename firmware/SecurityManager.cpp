#include "SecurityManager.h"

void SecurityManager::begin() {
  // TODO: implement
}

void SecurityManager::update(const SensorSnapshot& snapshot, SystemStateManager& stateManager) {
  // TODO: implement — on snapshot.securityButtonPressed, toggle arm
  // state; reject arm request if snapshot.irFault != ErrorCode::NONE;
  // on snapshot.motionDetected while armed, call
  // stateManager.requestTransition(SystemState::ALARM)
}

bool SecurityManager::isArmed() const {
  // TODO: implement
  return false;
}

bool SecurityManager::lastArmRequestRejected() const {
  // TODO: implement
  return false;
}

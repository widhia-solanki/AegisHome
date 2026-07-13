#include "SystemStateManager.h"

void SystemStateManager::begin() {
  // TODO: implement — initialize state_ to SystemState::BOOT
}

SystemState SystemStateManager::current() const {
  // TODO: implement
  return SystemState::BOOT;
}

bool SystemStateManager::requestTransition(SystemState next) {
  // TODO: implement — validate transition against allowed-transitions
  // table (see docs/03_FDS.md §6), log rejected requests, apply if valid
  return false;
}

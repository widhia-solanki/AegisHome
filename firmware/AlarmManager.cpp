#include "AlarmManager.h"

void AlarmManager::begin() {
  // TODO: implement — pinMode for buzzer/red LED, init pattern timer
}

void AlarmManager::update(SystemState currentState) {
  // TODO: implement — if currentState == SystemState::ALARM, step the
  // non-blocking buzzer/LED pattern; otherwise ensure both are off
}

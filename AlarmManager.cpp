#include "AlarmManager.h"

void AlarmManager::begin()
{
    pinMode(Config::PIN_BUZZER, OUTPUT);
    pinMode(Config::PIN_LED_INDICATOR, OUTPUT);

    digitalWrite(Config::PIN_BUZZER, LOW);
    digitalWrite(Config::PIN_LED_INDICATOR, LOW);

    patternStep_ = 0;
}

void AlarmManager::update(SystemState currentState) {
  // TODO: implement — if currentState == SystemState::ALARM, step the
  // non-blocking buzzer/LED pattern; otherwise ensure both are off
}

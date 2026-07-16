#ifndef AEGISHOME_ALARMMANAGER_H
#define AEGISHOME_ALARMMANAGER_H

#include <Arduino.h>
#include "config.h"
#include "Enums.h"
#include "Utilities.h"

// Owns buzzer pattern generation + red LED during ALARM state.
// Does not decide when to enter ALARM — SystemStateManager does that.
class AlarmManager {
public:
  void begin();
  void update(SystemState currentState);

private:
    Timer patternTimer_{Config::ALARM_PATTERN_STEP_MS};

    uint8_t patternStep_;
  };

#endif // AEGISHOME_ALARMMANAGER_H

#ifndef AEGISHOME_TEMPERATURECONTROLLER_H
#define AEGISHOME_TEMPERATURECONTROLLER_H

#include <Arduino.h>
#include "config.h"
#include "Types.h"
#include "Utilities.h"

// Compares filtered temperature to threshold (with hysteresis) and
// drives the "fan" LED + buzzer pulse. Does not know about security state.
// Reads from a SensorSnapshot, not SensorManager directly.
class TemperatureController {
public:
  void begin();
  void update(const SensorSnapshot& snapshot);

  bool isFanActive() const;

private:
    bool fanActive_;

    Timer buzzerTimer_{Config::OVERHEAT_BUZZ_DURATION_MS};
};

#endif // AEGISHOME_TEMPERATURECONTROLLER_H

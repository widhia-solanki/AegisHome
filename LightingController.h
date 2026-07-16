#ifndef AEGISHOME_LIGHTINGCONTROLLER_H
#define AEGISHOME_LIGHTINGCONTROLLER_H

#include <Arduino.h>
#include "config.h"
#include "Types.h"

// Drives lighting LED based on LDR comparator state. Does not know
// about alarm/security state. Fails "on" (safe default) if LDR faults.
// Reads from a SensorSnapshot, not SensorManager directly.
class LightingController {
public:
  void begin();
  void update(const SensorSnapshot& snapshot);

  bool isLightingOn() const;

private:
    bool lightingOn_;
};

#endif // AEGISHOME_LIGHTINGCONTROLLER_H

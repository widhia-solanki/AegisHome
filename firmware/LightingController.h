#ifndef AEGISHOME_LIGHTINGCONTROLLER_H
#define AEGISHOME_LIGHTINGCONTROLLER_H

#include <Arduino.h>
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
  // TODO: implement — lightingOn_ bool
};

#endif // AEGISHOME_LIGHTINGCONTROLLER_H

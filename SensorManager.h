#ifndef AEGISHOME_SENSORMANAGER_H
#define AEGISHOME_SENSORMANAGER_H

#include <Arduino.h>
#include "config.h"
#include "Utilities.h"
#include "Types.h"

// Owns all raw sensor/input reads, filtering, and debounce.
// Publishes exactly one SensorSnapshot per update() call — every
// controller reads from that same snapshot rather than calling live
// getters, so no controller can observe a value mid-change relative to
// another controller in the same loop iteration.
class SensorManager {
public:
  void begin();
  void update(); // call every loop iteration; refreshes the snapshot

  const SensorSnapshot& getSnapshot() const;

private:
  // TODO: implement — MovingAverage tempFilter, Debouncer instances for
  // IR/buttons, fault flags, and the SensorSnapshot instance itself
  SensorSnapshot snapshot_;
};

#endif // AEGISHOME_SENSORMANAGER_H

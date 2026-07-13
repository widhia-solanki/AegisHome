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
    // Filters
    MovingAverage tempFilter_{10};

    // Debounced digital inputs
    Debouncer armButton_;
    Debouncer doorbellButton_;

    // Sensor health flags
    bool thermistorHealthy_;
    bool ldrHealthy_;
    bool irHealthy_;

    // Shared snapshot published once per update()
    SensorSnapshot snapshot_;

    float readTemperatureC();
    bool readDarkState();
    bool readMotionState();
    bool checkSensorHealth();
};

#endif // AEGISHOME_SENSORMANAGER_H

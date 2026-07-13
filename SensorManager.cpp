#include "SensorManager.h"

void SensorManager::begin()
{
    // Configure analog sensor
    pinMode(Config::PIN_THERMISTOR, INPUT);

    // Configure digital sensors
    pinMode(Config::PIN_LDR_DIGITAL, INPUT);
    pinMode(Config::PIN_IR_MOTION, INPUT);

    // Initialize buttons
    doorbellButton_.begin(Config::PIN_BUTTON_DOORBELL, true);
    armButton_.begin(Config::PIN_BUTTON_SECURITY, true);

    // Sensor health flags
    thermistorHealthy_ = true;
    ldrHealthy_ = true;
    irHealthy_ = true;
}

void SensorManager::update() {
  // TODO: implement — poll analog/digital inputs on their own intervals,
  // update filters/debouncers, compute edge-triggered events, and write
  // the results into snapshot_ exactly once this cycle
}

const SensorSnapshot& SensorManager::getSnapshot() const {
  return snapshot_;
}

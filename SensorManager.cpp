#include "SensorManager.h"

void SensorManager::begin() {
  // TODO: implement — pinMode setup, filter/debouncer init, boot-time
  // plausibility check for thermistor/LDR (feeds boot diagnostics)
}

void SensorManager::update() {
  // TODO: implement — poll analog/digital inputs on their own intervals,
  // update filters/debouncers, compute edge-triggered events, and write
  // the results into snapshot_ exactly once this cycle
}

const SensorSnapshot& SensorManager::getSnapshot() const {
  return snapshot_;
}

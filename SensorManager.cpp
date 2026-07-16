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

bool SensorManager::readDarkState()
{
    // LM393 module:
    // LOW = Dark
    // HIGH = Bright
    return digitalRead(Config::PIN_LDR_DIGITAL) == LOW;
}

bool SensorManager::readMotionState()
{
    return digitalRead(Config::PIN_IR_MOTION) == HIGH;
}

float SensorManager::readTemperatureC()
{
    int adc = analogRead(Config::PIN_THERMISTOR);

    // TODO:
    // Convert ADC reading to resistance.
    // Convert resistance to temperature.
    // Filter using tempFilter_.
    // Return filtered temperature.

    return 0.0f;
}

bool SensorManager::checkSensorHealth()
{
    return true;
}

void SensorManager::update() {
  // TODO: implement — poll analog/digital inputs on their own intervals,
  // update filters/debouncers, compute edge-triggered events, and write
  // the results into snapshot_ exactly once this cycle
}

const SensorSnapshot& SensorManager::getSnapshot() const {
  return snapshot_;
}

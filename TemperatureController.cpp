#include "TemperatureController.h"
#include "config.h"
#include <Arduino.h>

void TemperatureController::begin()
{
    fanActive_ = false;

    // Reset timer so future timing features start clean.
    buzzerTimer_.reset();
}

void TemperatureController::update(const SensorSnapshot& snapshot)
{
    // Disable fan if thermistor has failed
    if (snapshot.thermistorFault != ErrorCode::NONE)
    {
        fanActive_ = false;
        digitalWrite(Config::PIN_FAN, LOW);
        return;
    }

    // Turn fan ON above threshold
    if (!fanActive_ &&
        snapshot.temperatureC >= Config::TEMP_THRESHOLD_C)
    {
        fanActive_ = true;
    }

    // Turn fan OFF below threshold - hysteresis
    else if (fanActive_ &&
             snapshot.temperatureC <=
             (Config::TEMP_THRESHOLD_C -
              Config::TEMP_HYSTERESIS_C))
    {
        fanActive_ = false;
    }

    digitalWrite(Config::PIN_FAN,
                 fanActive_ ? HIGH : LOW);
}

bool TemperatureController::isFanActive() const
{
    return fanActive_;
}

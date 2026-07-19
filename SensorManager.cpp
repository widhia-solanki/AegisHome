#include "SensorManager.h"
#include "Logger.h"
#include <math.h>

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
    return digitalRead(Config::PIN_IR_MOTION) == LOW;
}

float SensorManager::readTemperatureC()
{
    constexpr float SERIES_RESISTOR = 10000.0f;
    constexpr float NOMINAL_RESISTANCE = 10000.0f;
    constexpr float NOMINAL_TEMPERATURE = 25.0f;
    constexpr float BETA = 3950.0f;

    int adc = analogRead(Config::PIN_THERMISTOR);

    if (adc <= 0)
    {
        thermistorHealthy_ = false;
        return 0.0f;
    }

    thermistorHealthy_ = true;

    float resistance =
        SERIES_RESISTOR *
        ((1023.0f / adc) - 1.0f);

    float steinhart;

    steinhart = resistance / NOMINAL_RESISTANCE;
    steinhart = log(steinhart);
    steinhart /= BETA;
    steinhart += 1.0f / (NOMINAL_TEMPERATURE + 273.15f);
    steinhart = 1.0f / steinhart;
    steinhart -= 273.15f;

    tempFilter_.addSample(steinhart);

    return tempFilter_.getAverage();
}

bool SensorManager::checkSensorHealth()
{
    return true;
}

void SensorManager::update()
{
    armButton_.update();
    doorbellButton_.update();

    snapshot_.temperatureC = readTemperatureC();

    snapshot_.dark = readDarkState();

    snapshot_.motionDetected = readMotionState();

    if (snapshot_.motionDetected)
{
    LOG_INFO("IR", "Motion detected");
}

    snapshot_.doorbellPressed = doorbellButton_.justPressed();

    if (snapshot_.doorbellPressed)
{
    LOG_INFO("DOOR", "Doorbell pressed");
}

    snapshot_.securityButtonPressed = armButton_.justPressed();

if (snapshot_.securityButtonPressed)
{
    LOG_INFO("SECURITY", "Security button pressed");
}

    if (!thermistorHealthy_)
        snapshot_.thermistorFault = ErrorCode::E01_THERMISTOR_FAULT;
    else
        snapshot_.thermistorFault = ErrorCode::NONE;

    if (!ldrHealthy_)
        snapshot_.ldrFault = ErrorCode::E05_LDR_FAULT;
    else
        snapshot_.ldrFault = ErrorCode::NONE;

    if (!irHealthy_)
        snapshot_.irFault = ErrorCode::E02_IR_FAULT;
    else
        snapshot_.irFault = ErrorCode::NONE;

    static unsigned long lastDebug = 0;

if (millis() - lastDebug >= 1000)
{
    lastDebug = millis();

    Serial.print("ADC: ");
    Serial.print(analogRead(Config::PIN_THERMISTOR));

    Serial.print("  Temp: ");
    Serial.print(snapshot_.temperatureC);

    Serial.print("  Door:");
    Serial.print(snapshot_.doorbellPressed);

    Serial.print("  Security:");
    Serial.print(snapshot_.securityButtonPressed);

    Serial.print("  Motion:");
    Serial.println(snapshot_.motionDetected);
}
}

const SensorSnapshot& SensorManager::getSnapshot() const {
  return snapshot_;
}

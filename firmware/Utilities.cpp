#include "Utilities.h"

// ---------------------------------------------------------------------
// Debouncer
// ---------------------------------------------------------------------
Debouncer::Debouncer() {
  // TODO: implement
}

void Debouncer::begin(uint8_t pin, bool usePullup) {
  // TODO: implement
}

void Debouncer::update() {
  // TODO: implement
}

bool Debouncer::isHigh() const {
  // TODO: implement
  return false;
}

bool Debouncer::justPressed() const {
  // TODO: implement
  return false;
}

bool Debouncer::justReleased() const {
  // TODO: implement
  return false;
}

// ---------------------------------------------------------------------
// MovingAverage
// ---------------------------------------------------------------------
MovingAverage::MovingAverage(uint8_t windowSize) {
  // TODO: implement
}

void MovingAverage::addSample(float value) {
  // TODO: implement
}

float MovingAverage::getAverage() const {
  // TODO: implement
  return 0.0f;
}

// ---------------------------------------------------------------------
// Timer
// ---------------------------------------------------------------------

Timer::Timer(uint32_t intervalMs)
    : intervalMs_(intervalMs),
      lastTriggerMs_(millis())
{
}

bool Timer::isDue()
{
    uint32_t now = millis();

    if (now - lastTriggerMs_ >= intervalMs_)
    {
        lastTriggerMs_ = now;
        return true;
    }

    return false;
}

void Timer::reset()
{
    lastTriggerMs_ = millis();
}
}

bool Timer::isDue() {
  // TODO: implement
  return false;
}

void Timer::reset() {
  // TODO: implement
}

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

MovingAverage::MovingAverage(uint8_t windowSize)
    : windowSize_(min(windowSize, MAX_WINDOW)),
      index_(0),
      count_(0),
      sum_(0.0f)
{
    for (uint8_t i = 0; i < MAX_WINDOW; i++)
    {
        buffer_[i] = 0.0f;
    }
}

void MovingAverage::addSample(float value)
{
    if (count_ < windowSize_)
    {
        buffer_[index_] = value;
        sum_ += value;

        index_ = (index_ + 1) % windowSize_;
        count_++;
    }
    else
    {
        sum_ -= buffer_[index_];

        buffer_[index_] = value;

        sum_ += value;

        index_ = (index_ + 1) % windowSize_;
    }
}

float MovingAverage::getAverage() const
{
    if (count_ == 0)
    {
        return 0.0f;
    }

    return sum_ / count_;
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

bool Timer::isDue() {
  // TODO: implement
  return false;
}

void Timer::reset() {
  // TODO: implement
}

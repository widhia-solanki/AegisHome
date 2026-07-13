#include "Utilities.h"

// ---------------------------------------------------------------------
// Debouncer
// ---------------------------------------------------------------------
Debouncer::Debouncer()
    : pin_(0),
      usePullup_(false),
      lastStableState_(false),
      lastRawState_(false),
      justPressedFlag_(false),
      justReleasedFlag_(false),
      lastChangeMs_(0)
{
}

void Debouncer::begin(uint8_t pin, bool usePullup)
{
    pin_ = pin;
    usePullup_ = usePullup;

    pinMode(pin_, usePullup_ ? INPUT_PULLUP : INPUT);

    lastStableState_ = digitalRead(pin_);
    lastRawState_ = lastStableState_;

    justPressedFlag_ = false;
    justReleasedFlag_ = false;

    lastChangeMs_ = millis();
}

void Debouncer::update() {
  // TODO: implement
}

bool Debouncer::isHigh() const {
  // TODO: implement
  return false;
}

bool Debouncer::justPressed() {
  // TODO: implement
  return false;
}

bool Debouncer::justReleased() {
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
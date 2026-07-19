#ifndef AEGISHOME_UTILITIES_H
#define AEGISHOME_UTILITIES_H

#include <Arduino.h>

// Debounced digital input helper.
class Debouncer {
public:
  Debouncer();
  void begin(uint8_t pin, bool usePullup);
  void update();
  bool isHigh() const;
  bool justPressed();
  bool justReleased();

private:
  uint8_t pin_;
  bool usePullup_;

  bool lastStableState_;
  bool lastRawState_;

  bool justPressedFlag_;
  bool justReleasedFlag_;

  uint32_t lastChangeMs_;

  // Debounce interval comes from config.h
};

// Simple moving-average filter for noisy analog readings.
class MovingAverage {
public:
  explicit MovingAverage(uint8_t windowSize);
  void addSample(float value);
  float getAverage() const;

private:
  static constexpr uint8_t MAX_WINDOW = 20;

  float buffer_[MAX_WINDOW];

  uint8_t windowSize_;

  uint8_t index_;

  uint8_t count_;

  float sum_;
};

// Non-blocking interval timer.
class Timer {
public:
  explicit Timer(uint32_t intervalMs);
  bool isDue();   // returns true once per interval, resets internally
  void reset();

private:
  uint32_t intervalMs_;

  uint32_t lastTriggerMs_;
};

// NOTE (postponed — not v1): an EdgeDetector utility (wraps a raw boolean
// stream into justRose()/justFell() without owning debounce timing itself)
// is a reasonable future addition if event detection logic starts getting
// duplicated across SensorManager call sites. Not needed while Debouncer
// already covers every current digital input.

#endif // AEGISHOME_UTILITIES_H

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
  bool justPressed() const;   // rising edge, debounced
  bool justReleased() const;  // falling edge, debounced

private:
  // TODO: implement — pin, lastStableState, lastRawState, lastChangeMs
};

// Simple moving-average filter for noisy analog readings.
class MovingAverage {
public:
  explicit MovingAverage(uint8_t windowSize);
  void addSample(float value);
  float getAverage() const;

private:
  // TODO: implement — circular buffer, windowSize, sum
};

// Non-blocking interval timer.
class Timer {
public:
  explicit Timer(uint32_t intervalMs);
  bool isDue();   // returns true once per interval, resets internally
  void reset();

private:
  // TODO: implement — intervalMs, lastTriggerMs
};

// NOTE (postponed — not v1): an EdgeDetector utility (wraps a raw boolean
// stream into justRose()/justFell() without owning debounce timing itself)
// is a reasonable future addition if event detection logic starts getting
// duplicated across SensorManager call sites. Not needed while Debouncer
// already covers every current digital input.

#endif // AEGISHOME_UTILITIES_H

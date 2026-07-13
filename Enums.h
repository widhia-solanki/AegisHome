#ifndef AEGISHOME_ENUMS_H
#define AEGISHOME_ENUMS_H

#include <Arduino.h>

// =====================================================================
// Enums.h
// Shared enum types with no dependents of their own. Everything else
// (config.h, Types.h, SystemStateManager.h, DisplayManager.h, etc.)
// includes THIS instead of including each other for a single enum —
// keeps dependencies flowing one direction instead of forming a cycle.
// =====================================================================

enum class SystemState {
  BOOT,
  NORMAL,
  SECURITY_ARMED,
  ALARM
};

// Error codes — single source of truth, reused across OLED/Serial/
// future telemetry. See docs/03_FDS.md §11c.
enum class ErrorCode : uint8_t {
  NONE = 0,
  E01_THERMISTOR_FAULT,
  E02_IR_FAULT,
  E03_SERVO_FAULT,
  E04_OLED_FAULT,
  E05_LDR_FAULT
};

// Severity classification for any logged event or fault.
enum class Severity : uint8_t {
  INFO = 0,
  WARNING,
  ERROR,
  FATAL
};

#endif // AEGISHOME_ENUMS_H

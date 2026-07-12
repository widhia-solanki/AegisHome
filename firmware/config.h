#ifndef AEGISHOME_CONFIG_H
#define AEGISHOME_CONFIG_H

#include <Arduino.h>

// =====================================================================
// AegisHome — config.h
// Single source of truth for pins, thresholds, and timing constants.
// No other file should contain a bare numeric literal used for logic.
//
// PIN VALUES BELOW REFLECT THE HARDWARE CONFIGURATION FREEZE (HCF),
// NOT THE ORIGINAL PLAN. See docs/08_HCF.md — this file must match
// that document, not the Pin Allocation Matrix, once bring-up starts.
//
// Two corrections applied here (pending final HCF confirmation against
// your specific board):
//   1. D9/D10 are not real Dx-labeled GPIOs on a standard NodeMCU —
//      the status/welcome LED has been reassigned to D8, MERGED with
//      the alarm LED (one physical LED, pattern-differentiated: slow
//      blink = welcome/lighting, fast strobe = alarm) rather than
//      needing two separate output pins.
//   2. The buzzer caused issues on D3 (GPIO0, a boot-strapping pin) —
//      reassigned to RX (GPIO3). This sacrifices hardware Serial
//      receive capability, which AegisHome does not use (Logger only
//      transmits via Serial.println on TX). Confirm in HCF §3 row 2.
// =====================================================================

namespace Config {

// ---------------------------------------------------------------------
// Pin assignments — VALIDATED per docs/08_HCF.md (confirm before freeze)
// ---------------------------------------------------------------------
constexpr uint8_t PIN_THERMISTOR      = A0;  // sole ADC user
constexpr uint8_t PIN_LDR_DIGITAL     = D0;  // LM393 comparator digital out — confirm polarity in HCF
constexpr uint8_t PIN_IR_MOTION       = D5;  // confirm active-HIGH/LOW in HCF
constexpr uint8_t PIN_BUTTON_DOORBELL = D6;
constexpr uint8_t PIN_BUTTON_SECURITY = D7;
constexpr uint8_t PIN_SERVO_DOOR      = D4;  // boot pin — servo idles pre-setup(), verified in HCF
constexpr uint8_t PIN_LED_INDICATOR   = D8;  // MERGED status + welcome + alarm LED (was 2 pins, now 1)
constexpr uint8_t PIN_BUZZER          = RX;  // MOVED from D3 after boot-pin conflict (HCF row 2)
constexpr uint8_t PIN_OLED_SDA        = D2;
constexpr uint8_t PIN_OLED_SCL        = D1;

// ---------------------------------------------------------------------
// Thresholds & hysteresis
// ---------------------------------------------------------------------
constexpr float TEMP_THRESHOLD_C        = 30.0f; // fan LED trigger point
constexpr float TEMP_HYSTERESIS_C       = 1.5f;  // avoids flicker at boundary
constexpr uint16_t SERVO_ANGLE_CLOSED   = 0;
constexpr uint16_t SERVO_ANGLE_OPEN     = 90;

// ---------------------------------------------------------------------
// Timing (all non-blocking, millis()-based)
// ---------------------------------------------------------------------
constexpr uint32_t SENSOR_POLL_INTERVAL_MS      = 250;
constexpr uint32_t DISPLAY_REFRESH_INTERVAL_MS  = 500;
constexpr uint32_t ALARM_PATTERN_STEP_MS        = 100;
constexpr uint32_t DOOR_DWELL_TIME_MS           = 3000;
constexpr uint32_t OVERHEAT_BUZZ_DURATION_MS    = 2000;
constexpr uint32_t DEBOUNCE_INTERVAL_MS         = 40;
constexpr uint32_t BOOT_DIAGNOSTIC_STEP_MS      = 300;

// ---------------------------------------------------------------------
// OLED
// ---------------------------------------------------------------------
constexpr uint8_t OLED_I2C_ADDRESS = 0x3C; // confirm in HCF §3 row 1 — some
                                            // SSD1306 modules ship at 0x3D

// ---------------------------------------------------------------------
// Diagnostics / logging
// ---------------------------------------------------------------------
constexpr bool DEBUG_LOG_ENABLED = true;
constexpr uint32_t SERIAL_BAUD_RATE = 115200;

} // namespace Config

// ---------------------------------------------------------------------
// Error codes (single source of truth — reused across OLED/Serial/future
// telemetry). See docs/03_FDS.md §11c. Left outside Config:: since these
// are types used throughout the codebase, not tunable values.
// ---------------------------------------------------------------------
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

#endif // AEGISHOME_CONFIG_H

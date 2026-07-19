#ifndef AEGISHOME_CONFIG_H
#define AEGISHOME_CONFIG_H

#include <Arduino.h>
#include "Enums.h"

// =====================================================================
// AegisHome — config.h
// Single source of truth for pins, thresholds, and timing constants.
// No other file should contain a bare numeric literal used for logic.
//
// PIN VALUES BELOW ARE THE v1 HARDWARE FREEZE per docs/08_HCF.md.
// See docs/08_HCF.md for the reallocation rationale (buzzer D6,
// doorbell button D3, merged indicator LED D8).
//
// ErrorCode/Severity moved out to Enums.h (see that file) — config.h
// includes it so existing call sites (Config-adjacent code that expects
// ErrorCode/Severity to be reachable via "config.h") keep working
// unchanged.
// =====================================================================

namespace Config {

// ---------------------------------------------------------------------
// Pin assignments — v1 HARDWARE FREEZE, validated per docs/08_HCF.md
// ---------------------------------------------------------------------
constexpr uint8_t PIN_THERMISTOR      = A0;  // sole ADC user — validated
constexpr uint8_t PIN_LDR_DIGITAL     = D0;  // LM393 comparator — polarity validated
constexpr uint8_t PIN_OLED_SCL        = D1;  // validated, address 0x3C
constexpr uint8_t PIN_OLED_SDA        = D2;
constexpr uint8_t PIN_BUTTON_DOORBELL = D3;  // boot pin — INPUT_PULLUP idles HIGH, satisfies GPIO0 boot req
constexpr uint8_t PIN_SERVO_DOOR      = D4;  // boot pin — servo idles pre-setup(), sweep validated
constexpr uint8_t PIN_IR_MOTION       = D5;  // polarity validated
constexpr uint8_t PIN_BUZZER          = D6;  // plain GPIO, no boot role
constexpr uint8_t PIN_BUTTON_SECURITY = D7;  // INPUT_PULLUP validated
constexpr uint8_t PIN_LED_INDICATOR   = D8;  // boot pin — merged status+welcome+alarm LED, LOW-at-boot safe
constexpr uint8_t PIN_LIGHT_LED = 1;   // TX (GPIO1)
constexpr uint8_t PIN_FAN = 3;         // RX (GPIO3)
// ---------------------------------------------------------------------
// Thresholds & hysteresis
// ---------------------------------------------------------------------
constexpr float TEMP_THRESHOLD_C        = 35.0f; // fan LED trigger point
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
constexpr uint8_t OLED_I2C_ADDRESS = 0x3C; // validated via I2C scanner

// ---------------------------------------------------------------------
// Diagnostics / logging
// ---------------------------------------------------------------------
constexpr bool DEBUG_LOG_ENABLED = true;
constexpr uint32_t SERIAL_BAUD_RATE = 115200;

} // namespace Config

#endif // AEGISHOME_CONFIG_H

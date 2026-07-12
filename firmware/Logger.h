#ifndef AEGISHOME_LOGGER_H
#define AEGISHOME_LOGGER_H

#include <Arduino.h>
#include "config.h"

// Logging wrapper — the first fully-implemented module in AegisHome,
// by design: every other module depends on it for debugging, so it
// has to exist and work before anything else is worth bringing up.
//
// Today this only prints to Serial when Config::DEBUG_LOG_ENABLED is
// true. The macro layer exists so a future MQTT/network logging
// backend can be swapped in behind Logger::write() without touching
// any call site — every module calls LOG_INFO/LOG_WARN/LOG_ERROR
// instead of Serial.println() directly.
//
// Output format: [LEVEL][TAG] message
// e.g. [INFO][BOOT] Initializing OLED
//      [WARNING][LDR] Threshold adjusted
//      [ERROR][OLED] Initialization failed

namespace Logger {

inline const char* severityLabel(Severity level) {
  switch (level) {
    case Severity::INFO:    return "INFO";
    case Severity::WARNING: return "WARNING";
    case Severity::ERROR:   return "ERROR";
    case Severity::FATAL:   return "FATAL";
    default:                return "UNKNOWN";
  }
}

inline void begin() {
  if (Config::DEBUG_LOG_ENABLED) {
    Serial.begin(Config::SERIAL_BAUD_RATE);
    // Give the USB-serial bridge a moment to enumerate before the first
    // log line, so the earliest boot messages aren't lost on some boards.
    delay(50);
  }
}

inline void write(Severity level, const char* tag, const char* message) {
  if (!Config::DEBUG_LOG_ENABLED) {
    return;
  }
  Serial.print('[');
  Serial.print(severityLabel(level));
  Serial.print("][");
  Serial.print(tag);
  Serial.print("] ");
  Serial.println(message);
}

} // namespace Logger

#define LOGGER_BEGIN()       Logger::begin()
#define LOG_INFO(tag, msg)   Logger::write(Severity::INFO, tag, msg)
#define LOG_WARN(tag, msg)   Logger::write(Severity::WARNING, tag, msg)
#define LOG_ERROR(tag, msg)  Logger::write(Severity::ERROR, tag, msg)
#define LOG_FATAL(tag, msg)  Logger::write(Severity::FATAL, tag, msg)

#endif // AEGISHOME_LOGGER_H

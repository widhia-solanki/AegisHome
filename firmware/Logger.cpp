#include "Logger.h"

namespace Logger {

const char* severityLabel(Severity level) {
  switch (level) {
    case Severity::INFO:    return "INFO";
    case Severity::WARNING: return "WARNING";
    case Severity::ERROR:   return "ERROR";
    case Severity::FATAL:   return "FATAL";
    default:                return "UNKNOWN";
  }
}

void begin() {
  if (Config::DEBUG_LOG_ENABLED) {
    Serial.begin(Config::SERIAL_BAUD_RATE);
    // Give the USB-serial bridge a moment to enumerate before the first
    // log line, so the earliest boot messages aren't lost on some boards.
    delay(50);
  }
}

void write(Severity level, const char* tag, const char* message) {
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

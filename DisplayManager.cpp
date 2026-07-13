#include "DisplayManager.h"

void DisplayManager::begin() {
  // TODO: implement — Wire.begin(Config::PIN_OLED_SDA, Config::PIN_OLED_SCL),
  // OLED init, on failure set oledFault_ = ErrorCode::E04_OLED_FAULT and
  // fall back to Serial-only output per FDS §11d
}

void DisplayManager::renderBootStep(const char* label, bool ok) {
  // TODO: implement — print one line of the boot diagnostic checklist
  // (e.g. "Thermistor  [OK]") per FDS §11b
}

void DisplayManager::renderReady() {
  // TODO: implement — render "System Ready" screen
}

void DisplayManager::update(const SystemStatus& status) {
  // TODO: implement — render the 4-line status screen per FDS §11a using
  // only fields on `status` (state, fanActive, lightingOn, alarmActive,
  // doorOpen, error) — no direct reference to any feature controller
}

ErrorCode DisplayManager::getOledFault() const {
  // TODO: implement
  return ErrorCode::NONE;
}

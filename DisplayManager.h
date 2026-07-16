#ifndef AEGISHOME_DISPLAYMANAGER_H
#define AEGISHOME_DISPLAYMANAGER_H

#include <Arduino.h>
#include "config.h"
#include "Types.h"
#include <Adafruit_SSD1306.h>

// Read-only rendering of system state to OLED and/or Serial.
// Never mutates any state. Consumes a single SystemStatus snapshot —
// does not need to know about TemperatureController, LightingController,
// SecurityManager, or DoorController individually.
class DisplayManager {
public:
  void begin();
  void renderBootStep(const char* label, bool ok); // FDS §11b checklist line
  void renderReady();
  void update(const SystemStatus& status);

  ErrorCode getOledFault() const;

private:
    ErrorCode oledFault_ = ErrorCode::NONE;
};

#endif // AEGISHOME_DISPLAYMANAGER_H

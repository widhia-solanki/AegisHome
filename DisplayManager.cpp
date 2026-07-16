#include "DisplayManager.h"

void DisplayManager::begin()
{
    Wire.begin(
        Config::PIN_OLED_SDA,
        Config::PIN_OLED_SCL
    );

    if (!display_.begin(
            SSD1306_SWITCHCAPVCC,
            Config::OLED_I2C_ADDRESS))
    {
        oledFault_ = ErrorCode::E04_OLED_FAULT;
        return;
    }

    display_.clearDisplay();
    display_.setTextColor(SSD1306_WHITE);
    display_.setTextSize(1);
    display_.display();
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

ErrorCode DisplayManager::getOledFault() const
{
    return oledFault_;
}

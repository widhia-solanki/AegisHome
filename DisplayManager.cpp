#include "DisplayManager.h"

void DisplayManager::begin()
{
    Wire.begin(Config::PIN_OLED_SDA, Config::PIN_OLED_SCL);

    if (!display_.begin(
            SSD1306_SWITCHCAPVCC,
            Config::OLED_I2C_ADDRESS))
    {
        oledFault_ = ErrorCode::E04_OLED_FAULT;

        return;
    }

    display_.clearDisplay();
    display_.setTextSize(1);
    display_.setTextColor(SSD1306_WHITE);
    display_.display();
}

void DisplayManager::renderBootStep(
    const char* label,
    bool ok)
{
    if (oledFault_ != ErrorCode::NONE)
        return;

    display_.clearDisplay();

    display_.setCursor(0,0);

    display_.print(label);

    display_.print(" : ");

    display_.println(ok ? "OK" : "FAIL");

    display_.display();

}

void DisplayManager::renderReady()
{
    if (oledFault_ != ErrorCode::NONE)
        return;

    display_.clearDisplay();

    display_.setCursor(0,0);

    display_.println("AegisHome");

    display_.println("----------------");

    display_.println("System Ready");

    display_.display();
}

void DisplayManager::update(const SystemStatus& status)
{
    if (oledFault_ != ErrorCode::NONE)
        return;

    display_.clearDisplay();
    display_.setCursor(0, 0);

    display_.print("State: ");

    switch (status.state)
    {
        case SystemState::BOOT:
            display_.println("BOOT");
            break;

        case SystemState::NORMAL:
            display_.println("NORMAL");
            break;

        case SystemState::SECURITY_ARMED:
            display_.println("ARMED");
            break;

        case SystemState::ALARM:
            display_.println("ALARM");
            break;
    }

    display_.print("Fan : ");
    display_.println(status.fanActive ? "ON" : "OFF");

    display_.print("Light: ");
    display_.println(status.lightingOn ? "ON" : "OFF");

    display_.print("Door: ");
    display_.println(status.doorOpen ? "OPEN" : "CLOSED");

    display_.display();
}

ErrorCode DisplayManager::getOledFault() const
{
    return oledFault_;
}

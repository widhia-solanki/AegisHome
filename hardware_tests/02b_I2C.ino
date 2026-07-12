#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Wire.begin(D2, D1);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.begin(115200);
    Serial.println("OLED FAILED");
    while (true);
  }

  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5, 10);
  display.println("AEGIS");

  display.setTextSize(1);
  display.setCursor(18, 40);
  display.println("Home v1");

  display.display();
}

void loop() {
}
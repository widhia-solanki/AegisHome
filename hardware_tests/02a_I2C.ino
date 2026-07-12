#include <Wire.h>

void setup() {
  Wire.begin(D2, D1);   // SDA, SCL

  Serial.begin(115200);
  Serial.println("\nI2C Scanner");

  byte count = 0;

  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);

    if (Wire.endTransmission() == 0) {
      Serial.print("Found I2C device at 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);

      count++;
      delay(5);
    }
  }

  if (count == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("Scan complete.");
}

void loop() {
}
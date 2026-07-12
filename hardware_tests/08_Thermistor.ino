const int THERMISTOR_PIN = A0;

void setup() {
  Serial.begin(115200);
  Serial.println("\n===== Thermistor Test =====");
}

void loop() {

  int adc = analogRead(THERMISTOR_PIN);

  Serial.print("ADC Value: ");
  Serial.println(adc);

  delay(500);
}
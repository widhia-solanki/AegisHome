const int LDR_PIN = D7;

void setup() {
  pinMode(LDR_PIN, INPUT);
  Serial.begin(115200);
  Serial.println("===== LDR Test =====");
}

void loop() {
  bool dark = digitalRead(LDR_PIN);

  Serial.print("LDR State: ");

  if (dark)
    Serial.println("BRIGHT");
  else
    Serial.println("DARK");

  delay(300);
}
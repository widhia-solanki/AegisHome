const int BUTTON_PIN = D6;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println("Button Test");
}

void loop() {

  if (digitalRead(BUTTON_PIN) == LOW) {
    Serial.println("BUTTON PRESSED");
    delay(200);
  }

}
const int BUTTON_PIN = D6;
const int LED_PIN = D8;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
}

void loop() {

  bool pressed = digitalRead(BUTTON_PIN) == LOW;

  digitalWrite(LED_PIN, pressed);

  if (pressed) {
    Serial.println("BUTTON PRESSED");
    delay(200);
  }

}
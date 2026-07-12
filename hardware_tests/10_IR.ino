const int IR_PIN = D0;

void setup() {
  pinMode(IR_PIN, INPUT);
  Serial.begin(115200);
  Serial.println("===== IR Proximity Test =====");
}

void loop() {

  bool detected = digitalRead(IR_PIN);

  if (detected == LOW) {
    Serial.println("OBJECT DETECTED");
  } else {
    Serial.println("CLEAR");
  }

  delay(250);
}
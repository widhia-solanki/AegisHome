#include <Servo.h>

Servo doorServo;

void setup() {
  doorServo.attach(D4);
}

void loop() {
  doorServo.write(0);
  delay(1000);

  doorServo.write(90);
  delay(1000);

  doorServo.write(180);
  delay(1000);
}
#include <ESP32Servo.h>

static const int servoPin = 13;
static const int servoPin2 = 12;
Servo servo1;
Servo servo2;

void setup() {
  Serial.begin(115200);
  servo1.attach(servoPin);
  servo2.attach(servoPin2);
}

void loop() {
  // toDefaultPos();
  // swingLeftRight();
  // servo1.write(0);
  angle = servo1.read();
}

void toDefaultPos() {
  servo1.write(0);
  Serial.println("Default Pos Set");
}

void swingLeftRight() {
  // 90 is middle
  // Move right - left
  for(int posDegrees = 0; posDegrees <= 180; posDegrees++) {
    servo1.write(posDegrees);
    Serial.println(posDegrees);
    delay(5);
  }

  // Move left - right
  for(int posDegrees = 180; posDegrees >= 0; posDegrees--) {
    servo1.write(posDegrees);
    Serial.println(posDegrees);
    delay(5);
  }
}

#include <ESP32Servo.h>

// Pin definitions
static const int servoPin = 13;
static const int servoPin2 = 12;
static const int button1Pin = 14;  // Button for motion 1
static const int button2Pin = 15;  // Button for motion 2
static const int button3Pin = 16;  // Button for motion 3

// Servo objects
Servo servo1;
Servo servo2;

// Button states
int lastButton1State = HIGH;
int lastButton2State = HIGH;
int lastButton3State = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

// Motion state
bool isPlayingMotion = false;
unsigned long motionStartTime = 0;
int currentMotion = 0;
int currentStep = 0;

// Motion definitions
// Each motion is a sequence of steps: {servo1Angle, servo2Angle, duration}
// Duration is in milliseconds
const int MOTION_STEPS = 10;  // Maximum steps per motion
const int NUM_MOTIONS = 3;    // Number of predefined motions

// Motion 1: Ear twitch
const int motion1[MOTION_STEPS][3] = {
  {90, 90, 0},    // Start position
  {100, 100, 100}, // Quick twitch
  {90, 90, 500},   // Back to center
  {0, 0, 0}        // End of motion
};

// Motion 2: Ear wiggle
const int motion2[MOTION_STEPS][3] = {
  {90, 90, 0},    // Start position
  {80, 100, 200}, // Left ear down, right ear up
  {100, 80, 200}, // Left ear up, right ear down
  {80, 100, 200}, // Left ear down, right ear up
  {100, 80, 200}, // Left ear up, right ear down
  {90, 90, 500},   // Back to center
  {0, 0, 0}        // End of motion
};

// Motion 3: Ear rotation
const int motion3[MOTION_STEPS][3] = {
  {90, 90, 0},    // Start position
  {0, 0, 500},    // Both ears forward
  {180, 180, 500}, // Both ears backward
  {90, 90, 500},   // Back to center
  {0, 0, 0}        // End of motion
};

void setup() {
  Serial.begin(115200);
  
  // Initialize servos
  servo1.attach(servoPin);
  servo2.attach(servoPin2);
  
  // Initialize buttons
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
  
  // Move to default position
  servo1.write(90);
  servo2.write(90);
  
  Serial.println("Cat Ear Motion Controller initialized");
  Serial.println("Press buttons 1-3 to trigger different ear motions");
}

void loop() {
  // Check for button presses
  checkButton(button1Pin, lastButton1State, 1);
  checkButton(button2Pin, lastButton2State, 2);
  checkButton(button3Pin, lastButton3State, 3);
  
  // Update button states
  lastButton1State = digitalRead(button1Pin);
  lastButton2State = digitalRead(button2Pin);
  lastButton3State = digitalRead(button3Pin);
  
  // Play current motion if active
  if (isPlayingMotion) {
    playMotion();
  }
  
  // Small delay to avoid overwhelming the system
  delay(10);
}

void checkButton(int buttonPin, int &lastButtonState, int motionNum) {
  int buttonState = digitalRead(buttonPin);
  
  if (buttonState != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonState == LOW && !isPlayingMotion) {
      // Start the motion
      startMotion(motionNum);
    }
  }
}

void startMotion(int motionNum) {
  isPlayingMotion = true;
  currentMotion = motionNum;
  currentStep = 0;
  motionStartTime = millis();
  
  Serial.print("Starting motion ");
  Serial.println(motionNum);
  
  // Move to the first position of the motion
  const int (*motion)[3] = getMotion(motionNum);
  servo1.write(motion[0][0]);
  servo2.write(motion[0][1]);
}

void playMotion() {
  const int (*motion)[3] = getMotion(currentMotion);
  
  // Check if we've reached the end of the motion
  if (motion[currentStep][2] == 0) {
    isPlayingMotion = false;
    Serial.print("Motion ");
    Serial.print(currentMotion);
    Serial.println(" completed");
    return;
  }
  
  // Check if it's time to move to the next step
  unsigned long elapsedTime = millis() - motionStartTime;
  unsigned long stepStartTime = 0;
  
  // Calculate the start time of the current step
  for (int i = 0; i < currentStep; i++) {
    stepStartTime += motion[i][2];
  }
  
  // If enough time has passed, move to the next step
  if (elapsedTime >= stepStartTime + motion[currentStep][2]) {
    currentStep++;
    
    // Move to the next position
    servo1.write(motion[currentStep][0]);
    servo2.write(motion[currentStep][1]);
    
    Serial.print("Motion ");
    Serial.print(currentMotion);
    Serial.print(" step ");
    Serial.println(currentStep);
  }
}

const int (*getMotion(int motionNum))[3] {
  switch (motionNum) {
    case 1:
      return motion1;
    case 2:
      return motion2;
    case 3:
      return motion3;
    default:
      return motion1;  // Default to motion 1
  }
}

void toDefaultPos() {
  servo1.write(0);
  servo2.write(0);
  Serial.println("Default Pos Set");
}

void swingLeftRight() {
  // 90 is middle
  // Move right - left
  for(int posDegrees = 0; posDegrees <= 180; posDegrees++) {
    servo1.write(posDegrees);
    servo2.write(posDegrees);
    Serial.print("Angle: ");
    Serial.println(posDegrees);
    delay(5);
  }

  // Move left - right
  for(int posDegrees = 180; posDegrees >= 0; posDegrees--) {
    servo1.write(posDegrees);
    servo2.write(posDegrees);
    Serial.print("Angle: ");
    Serial.println(posDegrees);
    delay(5);
  }
}

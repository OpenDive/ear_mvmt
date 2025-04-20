#include <ESP32Servo.h>

// Pin definitions
static const int servoPin = 13;
static const int servoPin2 = 12;

// Servo objects
Servo servo1;
Servo servo2;

// Function prototypes
const int (*getMotion(int motionNum))[3];

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
  {90, 90, 100},   // Start position
  {100, 100, 100}, // Quick twitch
  {90, 90, 500},   // Back to center
  {0, 0, 0}        // End of motion
};

// Motion 2: Ear wiggle
const int motion2[MOTION_STEPS][3] = {
  {90, 90, 100},   // Start position
  {80, 100, 200}, // Left ear down, right ear up
  {100, 80, 200}, // Left ear up, right ear down
  {80, 100, 200}, // Left ear down, right ear up
  {100, 80, 200}, // Left ear up, right ear down
  {90, 90, 500},   // Back to center
  {0, 0, 0}        // End of motion
};

// Motion 3: Ear rotation
const int motion3[MOTION_STEPS][3] = {
  {90, 90, 100},   // Start position
  {0, 0, 500},    // Both ears forward
  {180, 180, 500}, // Both ears backward
  {90, 90, 500},   // Back to center
  {0, 0, 0}        // End of motion
};

// Serial command buffer
String inputString = "";
bool stringComplete = false;

void setup() {
  Serial.begin(115200);
  
  // Initialize servos
  servo1.attach(servoPin);
  servo2.attach(servoPin2);
  
  // Move to default position
  servo1.write(90);
  servo2.write(90);
  
  // Reserve 200 bytes for the inputString
  inputString.reserve(200);
  
  Serial.println("Cat Ear Motion Controller initialized");
  Serial.println("Available commands:");
  Serial.println("1 - Ear twitch");
  Serial.println("2 - Ear wiggle");
  Serial.println("3 - Ear rotation");
  Serial.println("s1,angle - Set servo 1 to angle (0-180)");
  Serial.println("s2,angle - Set servo 2 to angle (0-180)");
  Serial.println("c - Center both servos");
  Serial.println("h - Show this help message");
}

void loop() {
  // Check for serial data
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    
    // Add the incoming character to the string
    inputString += inChar;
    
    // If the incoming character is a newline, set a flag
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
  
  // Process serial commands
  if (stringComplete) {
    processCommand();
    inputString = "";
    stringComplete = false;
  }
  
  // Play current motion if active
  if (isPlayingMotion) {
    playMotion();
  }
  
  // Small delay to avoid overwhelming the system
  delay(10);
}

void processCommand() {
  // Trim whitespace and newlines
  inputString.trim();
  
  // Check for motion commands
  if (inputString == "1") {
    startMotion(1);
  } else if (inputString == "2") {
    startMotion(2);
  } else if (inputString == "3") {
    startMotion(3);
  } else if (inputString == "c") {
    // Center both servos
    servo1.write(90);
    servo2.write(90);
    Serial.println("Servos centered");
  } else if (inputString == "h") {
    // Show help message
    Serial.println("Available commands:");
    Serial.println("1 - Ear twitch");
    Serial.println("2 - Ear wiggle");
    Serial.println("3 - Ear rotation");
    Serial.println("s1,angle - Set servo 1 to angle (0-180)");
    Serial.println("s2,angle - Set servo 2 to angle (0-180)");
    Serial.println("c - Center both servos");
    Serial.println("h - Show this help message");
  } else if (inputString.startsWith("s1,")) {
    // Set servo 1 to specific angle
    int angle = inputString.substring(3).toInt();
    angle = constrain(angle, 0, 180);
    servo1.write(angle);
    Serial.print("Servo 1 set to ");
    Serial.println(angle);
  } else if (inputString.startsWith("s2,")) {
    // Set servo 2 to specific angle
    int angle = inputString.substring(3).toInt();
    angle = constrain(angle, 0, 180);
    servo2.write(angle);
    Serial.print("Servo 2 set to ");
    Serial.println(angle);
  } else {
    Serial.println("Unknown command. Type 'h' for help.");
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
  if (currentStep >= MOTION_STEPS || (motion[currentStep][0] == 0 && motion[currentStep][1] == 0 && motion[currentStep][2] == 0)) {
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
    
    // Check if we've reached the end of the motion
    if (currentStep >= MOTION_STEPS || (motion[currentStep][0] == 0 && motion[currentStep][1] == 0 && motion[currentStep][2] == 0)) {
      isPlayingMotion = false;
      Serial.print("Motion ");
      Serial.print(currentMotion);
      Serial.println(" completed");
      return;
    }
    
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

#include <ESP32Servo.h>

// Pin definitions
static const int servoPin = 13; // RIGHT ear from Robot's perspective; LEFT ear from user's perspective
static const int servoPin2 = 12; // LEFT ear from robot's perspective; RIGHT ear from user's perspective

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
const int MOTION_STEPS = 20;  // Maximum steps per motion
const int NUM_MOTIONS = 12;   // Increased number of predefined motions

// Motion 1: Ear twitch
const int motion1[MOTION_STEPS][3] = {
  {90, 90, 200},   // Start position
  {100, 100, 150}, // Quick twitch to the right
  {90, 90, 800},   // Back to center
  {0, 0, 0}        // End of motion
};

// Motion 2: Ear wiggle
const int motion2[MOTION_STEPS][3] = {
  {90, 90, 200},   // Start position
  {80, 100, 300},  // Left ear left, right ear right
  {100, 80, 300},  // Left ear right, right ear left
  {80, 100, 300},  // Left ear left, right ear right
  {100, 80, 300},  // Left ear right, right ear left
  {90, 90, 800},   // Back to center
  {0, 0, 0}        // End of motion
};

// Motion 3: Ear sweep
const int motion3[MOTION_STEPS][3] = {
  {90, 90, 200},   // Start position
  {0, 0, 800},    // Both ears to the left
  {180, 180, 800}, // Both ears to the right
  {90, 90, 800},   // Back to center
  {0, 0, 0}        // End of motion
};

// Motion 4: Alert ears
const int motion4[MOTION_STEPS][3] = {
  {90, 90, 200},   // Start position
  {60, 60, 400},   // Both ears to the left
  {60, 60, 2000},  // Hold position
  {90, 90, 400},   // Back to center
  {0, 0, 0}        // End of motion
};

// Motion 5: Relaxed ears
const int motion5[MOTION_STEPS][3] = {
  {90, 90, 200},   // Start position
  {120, 120, 400}, // Both ears to the right
  {120, 120, 2000}, // Hold position
  {90, 90, 400},   // Back to center
  {0, 0, 0}        // End of motion
};

// TODO: Review these motions, they might be wrong
// Motion 6: Curious tilt
const int motion6[MOTION_STEPS][3] = {
  {90, 90, 200},   // Start position
  {60, 120, 400},  // Left ear left, right ear right
  {60, 120, 2000}, // Hold position
  {90, 90, 400},   // Back to center
  {0, 0, 0}        // End of motion
};

// Motion 7: Startled ears
const int motion7[MOTION_STEPS][3] = {
  {90, 90, 200},   // Start position
  {30, 30, 100},   // Quick movement to the left
  {30, 30, 500},   // Hold startled position
  {90, 90, 400},   // Back to center
  {0, 0, 0}        // End of motion
};

// Motion 8: Happy ears
const int motion8[MOTION_STEPS][3] = {
  {90, 90, 200},   // Start position
  {80, 80, 300},   // Both ears slightly left
  {100, 100, 300}, // Both ears slightly right
  {80, 80, 300},   // Both ears slightly left
  {100, 100, 300}, // Both ears slightly right
  {80, 80, 300},   // Both ears slightly left
  {90, 90, 800},   // Back to center
  {0, 0, 0}        // End of motion
};

// Motion 9: Annoyed flick
const int motion9[MOTION_STEPS][3] = {
  {90, 90, 200},   // Start position
  {70, 70, 100},   // Quick flick to the left
  {110, 110, 100}, // Quick flick to the right
  {90, 90, 800},   // Back to center
  {0, 0, 0}        // End of motion
};

// Motion 10: Scanning ears
const int motion10[MOTION_STEPS][3] = {
  {90, 90, 200},   // Start position
  {45, 45, 500},   // Both ears to the left
  {45, 90, 800},   // Left ear left, right ear center
  {90, 45, 800},   // Right ear left, left ear center
  {45, 45, 800},   // Both ears to the left
  {90, 90, 500},   // Back to center
  {0, 0, 0}        // End of motion
};

// Motion 11: Sleepy ears
const int motion11[MOTION_STEPS][3] = {
  {90, 90, 200},   // Start position
  {130, 130, 400}, // Both ears to the right
  {130, 130, 2000}, // Hold position
  {90, 90, 400},   // Back to center
  {0, 0, 0}        // End of motion
};

// Motion 12: Greeting ears
const int motion12[MOTION_STEPS][3] = {
  {90, 90, 200},   // Start position
  {70, 70, 300},   // Both ears to the left
  {110, 110, 300}, // Both ears to the right
  {70, 70, 300},   // Both ears to the left
  {110, 110, 300}, // Both ears to the right
  {90, 90, 800},   // Back to center
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
  Serial.println("3 - Ear sweep");
  Serial.println("4 - Alert ears");
  Serial.println("5 - Relaxed ears");
  Serial.println("6 - Curious tilt");
  Serial.println("7 - Startled ears");
  Serial.println("8 - Happy ears");
  Serial.println("9 - Annoyed flick");
  Serial.println("10 - Scanning ears");
  Serial.println("11 - Sleepy ears");
  Serial.println("12 - Greeting ears");
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
  } else if (inputString == "4") {
    startMotion(4);
  } else if (inputString == "5") {
    startMotion(5);
  } else if (inputString == "6") {
    startMotion(6);
  } else if (inputString == "7") {
    startMotion(7);
  } else if (inputString == "8") {
    startMotion(8);
  } else if (inputString == "9") {
    startMotion(9);
  } else if (inputString == "10") {
    startMotion(10);
  } else if (inputString == "11") {
    startMotion(11);
  } else if (inputString == "12") {
    startMotion(12);
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
    Serial.println("3 - Ear sweep");
    Serial.println("4 - Alert ears");
    Serial.println("5 - Relaxed ears");
    Serial.println("6 - Curious tilt");
    Serial.println("7 - Startled ears");
    Serial.println("8 - Happy ears");
    Serial.println("9 - Annoyed flick");
    Serial.println("10 - Scanning ears");
    Serial.println("11 - Sleepy ears");
    Serial.println("12 - Greeting ears");
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
    case 4:
      return motion4;
    case 5:
      return motion5;
    case 6:
      return motion6;
    case 7:
      return motion7;
    case 8:
      return motion8;
    case 9:
      return motion9;
    case 10:
      return motion10;
    case 11:
      return motion11;
    case 12:
      return motion12;
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

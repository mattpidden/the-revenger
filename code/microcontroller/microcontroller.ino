#include <AccelStepper.h>
#include "TM1637.h"

#define rotateDirPin 31
#define rotateStepPin 22
#define rotateEnablePin 33
#define linearDirPin 29
#define linearStepPin 24
#define linearEnablePin 35

#define motorInterfaceType 1
#define limitPin 32

#define buzzerPin 7
#define displayClkPin 2
#define displayDioPin 3

TM1637 digitDisplay(displayClkPin, displayDioPin);
int8_t displayValues[] = {0, 0, 0, 0};

AccelStepper rotateStepper(motorInterfaceType, rotateStepPin, rotateDirPin);
AccelStepper linearStepper(motorInterfaceType, linearStepPin, linearDirPin);

int stepsPerRevolution = 200;
float pulleyCircumfrence = 39.0;
float stepsPerMM = stepsPerRevolution / pulleyCircumfrence;
float linearRailDistanceMM = 40; 

bool commandRunning = false;
bool lastCommandRunning = false;
char command = 'N';
unsigned long timeCommandStarted = micros();


void setup() {
  Serial.begin(9600); 
  pinMode(rotateEnablePin, OUTPUT); 
  pinMode(linearEnablePin, OUTPUT); 
  pinMode(limitPin, INPUT_PULLUP); 
  pinMode(buzzerPin, OUTPUT);

  digitDisplay.init();
  digitDisplay.set(BRIGHT_TYPICAL);
  
  digitalWrite(rotateEnablePin, HIGH);
  digitalWrite(linearEnablePin, HIGH);
  linearStepper.setMaxSpeed(25000.0);     
  linearStepper.setAcceleration(20000.0); 
  rotateStepper.setMaxSpeed(25000.0);     
  rotateStepper.setAcceleration(20000.0); 

  Serial.println("RUBIKS REVENGE ROBOT PROTOTYPE COMMANDS");
  Serial.println("'C' Calibrate linear acctuator");
  Serial.println("'O' Send linear acctuator out");
  Serial.println("'I' Send linear acctuator in");
  Serial.println("'R' Rotate claw");
  Serial.println("'F' Perform a full action");
  Serial.println("");
}

void loop() {
  if (commandRunning) {
    updateDisplay();
  } else if (commandRunning == false && lastCommandRunning == true) {
    soundBuzzer();
  } else if (Serial.available() > 0) {
    command = Serial.read(); 
  }
  
  lastCommandRunning = commandRunning;

  if (command == 'C') {
    calibrateLinearAcctuator(); 
  }
  else if (command == 'O') {
    moveLinearHubOut(); 
  }
  else if (command == 'I') {
    moveLinearHubIn(); 
  }
  else if (command == 'R') {
    rotateClaw(); 
  }
  else if (command == 'F') {
    fullAction(); 
  }
}

void soundBuzzer() {
  digitalWrite(buzzerPin, HIGH);
  delay(100);
  digitalWrite(buzzerPin, LOW);
}

void updateDisplay() {
  unsigned long nowTime = micros();
  unsigned long durationMicroseconds = nowTime - timeCommandStarted;
  float durationSeconds = durationMicroseconds / 1000000.0;
  int sec = (int)durationSeconds;                      
  int ms = (int)((durationSeconds - sec) * 100);   
  int8_t displayValues[] = {static_cast<int8_t>(sec / 10), static_cast<int8_t>(sec % 10), static_cast<int8_t>(ms / 10), static_cast<int8_t>(ms % 10)};

  digitDisplay.point(POINT_ON);
  digitDisplay.display(displayValues);
}

void calibrateLinearAcctuator() {
  digitalWrite(linearEnablePin, LOW);
  linearStepper.setMaxSpeed(100.0);     
  while (digitalRead(limitPin) == HIGH) { 
    linearStepper.move(-1); 
    linearStepper.run();
  }
  linearStepper.stop();
  linearStepper.setCurrentPosition(0);
  int stepsToMove = 2 * stepsPerMM;
  linearStepper.move(stepsToMove);
  linearStepper.runToPosition();
  linearStepper.setMaxSpeed(25000.0);     
  digitalWrite(linearEnablePin, HIGH);

}

void moveLinearHubOut() {
  if (!commandRunning) {
    soundBuzzer();
    commandRunning = true;
    timeCommandStarted = micros();
    digitalWrite(linearEnablePin, LOW);
    int stepsToMove = linearRailDistanceMM * stepsPerMM;
    linearStepper.move(stepsToMove);
    linearStepper.run();
  } 
  if (linearStepper.distanceToGo() == 0) {
    digitalWrite(linearEnablePin, HIGH);
    commandRunning = false;
    command = 'N';
  } else {
    linearStepper.run();
  }
}

void moveLinearHubIn() {
  if (!commandRunning) {
    soundBuzzer();
    commandRunning = true;
    timeCommandStarted = micros();
    digitalWrite(linearEnablePin, LOW);
    int stepsToMove = linearRailDistanceMM * stepsPerMM;
    linearStepper.move(-stepsToMove);
    linearStepper.run();
  } 
  if (linearStepper.distanceToGo() == 0) {
    digitalWrite(linearEnablePin, HIGH);
    commandRunning = false;
    command = 'N';
  } else {
    linearStepper.run();
  }
}

void rotateClaw() {
  if (!commandRunning) {
    soundBuzzer();
    commandRunning = true;
    timeCommandStarted = micros();
    digitalWrite(rotateEnablePin, LOW);
    rotateStepper.move(100);
    rotateStepper.run();
  } 
  if (rotateStepper.distanceToGo() == 0) {
    digitalWrite(rotateEnablePin, HIGH);
    commandRunning = false;
    command = 'N';
  } else {
    rotateStepper.run();
  }
}

void fullAction() {
  soundBuzzer();
  
  unsigned long startTime = micros(); 

  moveLinearHubOut();
  rotateClaw();
  moveLinearHubIn();

  unsigned long endTime = micros();
  unsigned long durationMicroseconds = endTime - startTime;
  float durationMilliseconds = durationMicroseconds / 1000.0;
  float durationSeconds = durationMicroseconds / 1000000.0;

  soundBuzzer();

  Serial.print("Execution Time: ");
  Serial.print(durationMicroseconds);
  Serial.print(" µs | ");
  Serial.print(durationMilliseconds);
  Serial.print(" ms | ");
  Serial.print(durationSeconds);
  Serial.println(" s");
}

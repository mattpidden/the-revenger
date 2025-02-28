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

AccelStepper rotateStepper(motorInterfaceType, rotateStepPin, rotateDirPin);
AccelStepper linearStepper(motorInterfaceType, linearStepPin, linearDirPin);

int stepsPerRevolution = 200;
float pulleyCircumfrence = 39.0;
float stepsPerMM = stepsPerRevolution / pulleyCircumfrence;
float linearRailDistanceMM = 40; 

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
  linearStepper.setMaxSpeed(2500.0);     
  linearStepper.setAcceleration(20000.0); 
  rotateStepper.setMaxSpeed(2500.0);     
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
  if (Serial.available() > 0) {
    char command = Serial.read(); 
    if (command == 'C') {
      Serial.println("'C' Calibrate linear acctuator");
      calibrateLinearAcctuator(); 
    }
    else if (command == 'O') {
      Serial.println("'O' Send linear acctuator out");
      moveLinearHubOut(); 
    }
    else if (command == 'I') {
      Serial.println("'I' Send linear acctuator in");
      moveLinearHubIn(); 
    }
    else if (command == 'R') {
      Serial.println("'R' Rotate claw");
      rotateClaw(); 
    }
    else if (command == 'F') {
      Serial.println("'F' Perform a full action");
      fullAction(); 
    }
  }
}

void soundBuzzer() {
  digitalWrite(buzzerPin, HIGH);
  delay(100);
  digitalWrite(buzzerPin, LOW);
}

void updateDisplay(unsigned long startTime, unsigned long endTime) {
  unsigned long durationMicroseconds = endTime - startTime;
  float durationMilliseconds = durationMicroseconds / 1000.0;
  float durationSeconds = durationMicroseconds / 1000000.0;
  int sec = (int)durationSeconds;                      
  int ms = (int)((durationSeconds - sec) * 100);   
  int8_t displayValues[] = {static_cast<int8_t>(sec / 10), static_cast<int8_t>(sec % 10), static_cast<int8_t>(ms / 10), static_cast<int8_t>(ms % 10)};

  digitDisplay.point(POINT_ON);
  digitDisplay.display(displayValues);
  
}

void resetDisplay() {
  digitDisplay.clearDisplay();
  digitDisplay.point(POINT_OFF);
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
  digitalWrite(linearEnablePin, LOW);
  int stepsToMove = linearRailDistanceMM * stepsPerMM;
  linearStepper.move(stepsToMove);
  linearStepper.runToPosition();
  digitalWrite(linearEnablePin, HIGH);
}

void moveLinearHubIn() {
  digitalWrite(linearEnablePin, LOW);
  int stepsToMove = linearRailDistanceMM * stepsPerMM;
  linearStepper.move(-stepsToMove);
  linearStepper.runToPosition();
  digitalWrite(linearEnablePin, HIGH);
}

void rotateClaw() {
  digitalWrite(rotateEnablePin, LOW);
  rotateStepper.move(100);
  rotateStepper.runToPosition();
  digitalWrite(rotateEnablePin, HIGH);
}

void fullAction() {
  soundBuzzer();
  
  unsigned long startTime = micros(); 
  unsigned long endTime = micros();
  updateDisplay(startTime, endTime);
  
  for (int i = 0; i < 1; i++) {
    moveLinearHubOut();
    endTime = micros();
    updateDisplay(startTime, endTime);
    
    rotateClaw();
    endTime = micros();
    updateDisplay(startTime, endTime);
    
    moveLinearHubIn();
    endTime = micros();
    updateDisplay(startTime, endTime);
  }
  

  

  soundBuzzer();

  delay(500);
  resetDisplay();
  delay(500);
  updateDisplay(startTime, endTime);
  delay(500);
  resetDisplay();
  delay(500);
  updateDisplay(startTime, endTime);
  delay(500);
  resetDisplay();
  delay(500);
  updateDisplay(startTime, endTime);
  delay(500);
  resetDisplay();
  delay(500);
  updateDisplay(startTime, endTime);
}

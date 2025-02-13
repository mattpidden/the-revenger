#include <AccelStepper.h>

#define rotateDirPin 13
#define rotateStepPin 12
#define rotateEnablePin 11
#define linearDirPin 10
#define linearStepPin 9
#define linearEnablePin 8
#define motorInterfaceType 1
#define limitPin 7

AccelStepper rotateStepper(motorInterfaceType, rotateStepPin, rotateDirPin);
AccelStepper linearStepper(motorInterfaceType, linearStepPin, linearDirPin);

int stepsPerRevolution = 200;
float pulleyCircumfrence = 39.0;
float stepsPerMM = stepsPerRevolution / pulleyCircumfrence;
float linearRailDistanceMM = 40; 

void setup() {
  Serial.begin(9600); 
  pinMode(rotateEnablePin, OUTPUT); 
  digitalWrite(rotateEnablePin, HIGH);
  pinMode(linearEnablePin, OUTPUT); 
  digitalWrite(linearEnablePin, HIGH);
  pinMode(limitPin, INPUT_PULLUP); 
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
  unsigned long startTime = micros(); 

  moveLinearHubOut();
  rotateClaw();
  moveLinearHubIn();

  unsigned long endTime = micros();
  unsigned long durationMicroseconds = endTime - startTime;
  float durationMilliseconds = durationMicroseconds / 1000.0;
  float durationSeconds = durationMicroseconds / 1000000.0;

  Serial.print("Execution Time: ");
  Serial.print(durationMicroseconds);
  Serial.print(" µs | ");
  Serial.print(durationMilliseconds);
  Serial.print(" ms | ");
  Serial.print(durationSeconds);
  Serial.println(" s");
}

#include <AccelStepper.h>
#include "TM1637.h"

#define x_axis_1_limit 32

#define x_axis_2_la_dir 25
#define x_axis_2_la_step 28
#define x_axis_2_la_enable 6
#define x_axis_2_limit 34

#define z_axis_1_la_dir 27
#define z_axis_1_la_step 26
#define z_axis_1_la_enable 5

#define z_axis_2_la_dir 29
#define z_axis_2_la_step 24
#define z_axis_2_la_enable 33

#define y_axis_1_la_dir 31
#define y_axis_1_la_step 22
#define y_axis_1_la_enable 35

#define buzzerPin 7
#define displayClkPin 2
#define displayDioPin 3

TM1637 digitDisplay(displayClkPin, displayDioPin);

#define motorInterfaceType 1
AccelStepper x_axis_1_la(motorInterfaceType, x_axis_2_la_step, x_axis_2_la_dir);
AccelStepper x_axis_1_claw(motorInterfaceType, z_axis_1_la_step, z_axis_1_la_dir);
AccelStepper x_axis_2_la(motorInterfaceType, z_axis_2_la_step, z_axis_2_la_dir);
AccelStepper x_axis_2_claw(motorInterfaceType, y_axis_1_la_step, y_axis_1_la_dir);

int stepsPerRevolution = 200;
float pulleyCircumfrence = 40.0;
float stepsPerMM = stepsPerRevolution / pulleyCircumfrence;
float linearRailDistanceMM = 62.2;
float linearRailOffsetMM = 2.2;
float distanceToDepth3 = 57.0;
float distanceToDepth2 = 44.0;
float distanceToDepth1 = 31.0; 

float linear_motor_max_speed = 25000.0;
float claw_motor_max_speed = 25000.0;
float linear_motor_acceleration = 20000.0;
float claw_motor_acceleration = 20000.0;

void setup() {
  Serial.begin(9600); 
  // pinMode(x_axis_2_enable, OUTPUT); 
  // pinMode(z_axis_1_enable, OUTPUT); 
  // pinMode(z_axis_2_enable, OUTPUT); 
  // pinMode(y_axis_1_enable, OUTPUT); 
  pinMode(x_axis_1_limit, INPUT_PULLUP); 
  pinMode(x_axis_2_limit, INPUT_PULLUP); 
  pinMode(buzzerPin, OUTPUT);

  digitDisplay.init();
  digitDisplay.set(BRIGHT_TYPICAL);
  
  // digitalWrite(x_axis_2_enable, HIGH);
  // digitalWrite(z_axis_1_enable, HIGH);
  // digitalWrite(z_axis_2_enable, HIGH);
  // digitalWrite(y_axis_1_enable, HIGH);

  x_axis_1_la.setMaxSpeed(linear_motor_max_speed);     
  x_axis_1_la.setAcceleration(linear_motor_acceleration); 
  x_axis_1_claw.setMaxSpeed(claw_motor_max_speed);     
  x_axis_1_claw.setAcceleration(claw_motor_acceleration); 
  x_axis_2_la.setMaxSpeed(linear_motor_max_speed);     
  x_axis_2_la.setAcceleration(linear_motor_acceleration); 
  x_axis_2_claw.setMaxSpeed(claw_motor_max_speed);     
  x_axis_2_claw.setAcceleration(claw_motor_acceleration); 

  Serial.println("RUBIKS REVENGE ROBOT PROTOTYPE COMMANDS");
  Serial.println("'C' Calibrate linear acctuator");
  Serial.println("'O' Send linear acctuator out");
  Serial.println("'I' Send linear acctuator in");
  Serial.println("'R' Rotate claw");
  Serial.println("'F' Perform a full action");
  Serial.println("");
  soundBuzzer();
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
      moveLinearHubOut(x_axis_2_la, 2); 
    }
    else if (command == 'I') {
      Serial.println("'I' Send linear acctuator in");
      moveLinearHubIn(x_axis_2_la, 2); 
    }
    else if (command == 'R') {
      Serial.println("'R' Rotate claw");
      rotateClaw(4); 
    }
    else if (command == 'F') {
      Serial.println("'F' Perform a full action");
      fullAction(); 
    }
    else if (command == 'B') {
      Serial.println("'B' BUZZER");
      soundBuzzer(); 
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
  int stepsToMove = linearRailOffsetMM * stepsPerMM;

  // digitalWrite(linearEnablePin, LOW);
  while (digitalRead(x_axis_2_limit) == HIGH) { 
    x_axis_2_la.move(-1); 
    x_axis_2_la.run();
  }
  x_axis_2_la.stop();
  x_axis_2_la.setCurrentPosition(0);
  x_axis_2_la.move(stepsToMove);
  x_axis_2_la.runToPosition();
  x_axis_2_la.setMaxSpeed(linear_motor_max_speed);     
  // digitalWrite(linearEnablePin, HIGH);
  
  // digitalWrite(linearEnablePin, LOW);
  while (digitalRead(x_axis_1_limit) == HIGH) { 
    x_axis_1_la.move(-1); 
    x_axis_1_la.run();
  }
  x_axis_1_la.stop();
  x_axis_1_la.setCurrentPosition(0);
  x_axis_1_la.move(stepsToMove);
  x_axis_1_la.runToPosition();
  x_axis_1_la.setMaxSpeed(linear_motor_max_speed);     
  // digitalWrite(linearEnablePin, HIGH);
}

void moveLinearHubOut(AccelStepper motor, int depth) {
  int stepsToMove = 0;
  if (depth == 1) {
    stepsToMove = distanceToDepth1 * stepsPerMM;
  } else if (depth == 2) {
    stepsToMove = distanceToDepth2 * stepsPerMM;
  } else if (depth == 3) {
    stepsToMove = distanceToDepth3 * stepsPerMM;
  }
  // digitalWrite(linearEnablePin, LOW);
  motor.move(stepsToMove);
  motor.runToPosition();
  // digitalWrite(linearEnablePin, HIGH);
}

void move2LinearHubOut(AccelStepper motor1, AccelStepper motor2, int depth) {
  int stepsToMove = 0;
  if (depth == 1) {
    stepsToMove = distanceToDepth1 * stepsPerMM;
  } else if (depth == 2) {
    stepsToMove = distanceToDepth2 * stepsPerMM;
  } else if (depth == 3) {
    stepsToMove = distanceToDepth3 * stepsPerMM;
  }
  motor1.move(stepsToMove);
  motor2.move(stepsToMove);
 
  while (motor1.distanceToGo() != 0 || motor2.distanceToGo() != 0) {
    motor1.run();
    motor2.run();
  }
}

void moveLinearHubIn(AccelStepper motor, int depth) {
  int stepsToMove = 0;
  if (depth == 1) {
    stepsToMove = distanceToDepth1 * stepsPerMM;
  } else if (depth == 2) {
    stepsToMove = distanceToDepth2 * stepsPerMM;
  } else if (depth == 3) {
    stepsToMove = distanceToDepth3 * stepsPerMM;
  }
  // digitalWrite(linearEnablePin, LOW);
  motor.move(-stepsToMove);
  motor.runToPosition();
  // digitalWrite(linearEnablePin, HIGH);
}

void rotateClaw(int turns) {
  // digitalWrite(rotateEnablePin, LOW);
  x_axis_2_claw.move(50 * turns);
  x_axis_2_claw.runToPosition();
  // digitalWrite(rotateEnablePin, HIGH);
}

void fullAction() {
  soundBuzzer();
  
  unsigned long startTime = micros(); 
  unsigned long endTime = micros();
  updateDisplay(startTime, endTime);

  moveLinearHubOut(x_axis_1_la, 2);
  moveLinearHubOut(x_axis_2_la, 2);
  rotateClaw(1);
  moveLinearHubIn(x_axis_2_la, 2);
  moveLinearHubIn(x_axis_1_la, 2);
  
  endTime = micros();
  updateDisplay(startTime, endTime);
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

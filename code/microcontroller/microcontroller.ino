#include <AccelStepper.h>
#include <array>
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

struct LinearMotor {
    AccelStepper* stepper;
    int limit_pin;
    int enable_pin;
};

struct ClawMotor {
    AccelStepper* stepper;
    int enable_pin;
};

#define motorInterfaceType 1
AccelStepper x_axis_1_la_stepper(motorInterfaceType, x_axis_2_la_step, x_axis_2_la_dir);
AccelStepper x_axis_1_claw_stepper(motorInterfaceType, z_axis_1_la_step, z_axis_1_la_dir);
AccelStepper x_axis_2_la_stepper(motorInterfaceType, z_axis_2_la_step, z_axis_2_la_dir);
AccelStepper x_axis_2_claw_stepper(motorInterfaceType, y_axis_1_la_step, y_axis_1_la_dir);

LinearMotor x_axis_1_la = {&x_axis_1_la_stepper, x_axis_1_limit, x_axis_2_la_enable};
ClawMotor x_axis_1_claw = {&x_axis_1_claw_stepper, z_axis_1_la_enable};
LinearMotor x_axis_2_la = {&x_axis_2_la_stepper, x_axis_2_limit, z_axis_2_la_enable};
ClawMotor x_axis_2_claw = {&x_axis_2_claw_stepper, y_axis_1_la_enable};

std::array<LinearMotor, 2> la_motors = {x_axis_1_la, x_axis_2_la};
std::array<ClawMotor, 2> claw_motors = {x_axis_1_claw, x_axis_2_claw};

int stepsPerRevolution = 200;
float pulleyCircumfrence = 40.0;
float stepsPerMM = stepsPerRevolution / pulleyCircumfrence;
float linearRailDistanceMM = 62.2;
float linearRailOffsetMM = 3;
float distanceToDepth1 = 31.0; 
float distanceToDepth2 = 44.0;
float distanceToDepth3 = 57.0;
int stepsToDepth1 = distanceToDepth1 * stepsPerMM;
int stepsToDepth2 = distanceToDepth2 * stepsPerMM;
int stepsToDepth3 = distanceToDepth3 * stepsPerMM;

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

  x_axis_1_la_stepper.setMaxSpeed(linear_motor_max_speed);     
  x_axis_1_la_stepper.setAcceleration(linear_motor_acceleration); 
  x_axis_1_claw_stepper.setMaxSpeed(claw_motor_max_speed);     
  x_axis_1_claw_stepper.setAcceleration(claw_motor_acceleration); 
  x_axis_2_la_stepper.setMaxSpeed(linear_motor_max_speed);     
  x_axis_2_la_stepper.setAcceleration(linear_motor_acceleration); 
  x_axis_2_claw_stepper.setMaxSpeed(claw_motor_max_speed);     
  x_axis_2_claw_stepper.setAcceleration(claw_motor_acceleration); 

  Serial.println("RUBIKS REVENGE ROBOT PROTOTYPE COMMANDS");
  Serial.println("'C' Calibrate linear acctuators");
  Serial.println("'F' Perform a full action");
  Serial.println("'B' Buzzer Sound");
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
    else if (command == 'F') {
      Serial.println("'F' Perform a full action");
      fullAction(); 
    }
    else if (command == 'B') {
      Serial.println("'B' Buzzer Sound");
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

  for (LinearMotor &motor : la_motors) {
     // digitalWrite(motor.enable_pin, LOW);
    while (digitalRead(motor.limit_pin) == HIGH) { 
      motor.stepper->move(-5); 
      motor.stepper->run();
    }
    motor.stepper->stop();
    motor.stepper->setCurrentPosition(0);
    motor.stepper->move(stepsToMove);
    motor.stepper->runToPosition();
    // digitalWrite(motor.enable_pin, HIGH);
  }
}

void moveLinearHubOut(LinearMotor motor, int steps) {
  // digitalWrite(motor.enable_pin, LOW);
  motor.stepper->move(steps);
  motor.stepper->runToPosition();
  // digitalWrite(motor.enable_pin, HIGH);
}

void moveLinearHubIn(LinearMotor motor, int steps) {
  // digitalWrite(motor.enable_pin, LOW);
  motor.stepper->move(-steps);
  motor.stepper->runToPosition();
  // digitalWrite(motor.enable_pin, HIGH);
}

void move2LinearHubOut(LinearMotor motor1, LinearMotor motor2, int steps1, int steps2) {
  motor1.stepper->move(steps1);
  motor2.stepper->move(steps2);
 
  while (motor1.stepper->distanceToGo() != 0 || motor2.stepper->distanceToGo() != 0) {
    motor1.stepper->run();
    motor2.stepper->run();
  }
}

void move2LinearHubIn(LinearMotor motor1, LinearMotor motor2, int steps1, int steps2) {
  motor1.stepper->move(-steps1);
  motor2.stepper->move(-steps2);
 
  while (motor1.stepper->distanceToGo() != 0 || motor2.stepper->distanceToGo() != 0) {
    motor1.stepper->run();
    motor2.stepper->run();
  }
}

void rotateClaw(ClawMotor motor, int turns) {
  // digitalWrite(motor.enable_pin, LOW);
  motor.stepper->move(50 * turns);
  motor.stepper->runToPosition();
  // digitalWrite(motor.enable_pin, HIGH);
}

void rotateClaws(ClawMotor motor1, ClawMotor motor2, int turns) {
  if (turns == 1) {
    rotateClaw(motor1, 1);
  } else if (turns <= 3) {
    motor1.stepper->move(50);
    motor2.stepper->move(-(turns - 1) * 50);
    while (motor1.stepper->distanceToGo() != 0 || motor2.stepper->distanceToGo() != 0) {
      motor1.stepper->run();
      motor2.stepper->run();
    }
  } else if (turns == 4) {
    motor1.stepper->move(100);
    motor2.stepper->move(-100);
    while (motor1.stepper->distanceToGo() != 0 || motor2.stepper->distanceToGo() != 0) {
      motor1.stepper->run();
      motor2.stepper->run();
    }
  }
}

void fullAction() {
  soundBuzzer();
  
  unsigned long startTime = micros(); 
  unsigned long endTime = micros();
  updateDisplay(startTime, endTime);

  move2LinearHubOut(x_axis_1_la, x_axis_2_la, stepsToDepth2, stepsToDepth2);
  rotateClaws(x_axis_2_claw, FF, 2);
  move2LinearHubIn(x_axis_1_la, x_axis_2_la, stepsToDepth2, stepsToDepth2);
  
  
  endTime = micros();
  updateDisplay(startTime, endTime);
  soundBuzzer();
}

#include <AccelStepper.h>
#include <array>
#include "TM1637.h"

// ***************************************************
// ********************* PINS ************************
// ***************************************************
#define x_axis_1_la_dir 23
#define x_axis_1_la_step 30
#define x_axis_1_la_enable 13
#define x_axis_1_la_limit 32

#define x_axis_2_la_dir 25
#define x_axis_2_la_step 28
#define x_axis_2_la_enable 10
#define x_axis_2_la_limit 34

#define z_axis_1_la_dir 27
#define z_axis_1_la_step 26
#define z_axis_1_la_enable 9
#define z_axis_1_la_limit 40

#define z_axis_2_la_dir 29
#define z_axis_2_la_step 24
#define z_axis_2_la_enable 8
#define z_axis_2_la_limit 42

#define y_axis_1_la_dir 31
#define y_axis_1_la_step 22
#define y_axis_1_la_enable 33
#define y_axis_1_la_limit 36

#define y_axis_2_la_dir 53
#define y_axis_2_la_step 52
#define y_axis_2_la_enable 41
#define y_axis_2_la_limit 38

#define x_axis_1_claw_dir 5
#define x_axis_1_claw_step 6
#define x_axis_1_claw_enable 39

#define x_axis_2_claw_dir 15
#define x_axis_2_claw_step 14
#define x_axis_2_claw_enable 37

#define z_axis_1_claw_dir 49
#define z_axis_1_claw_step 48
#define z_axis_1_claw_enable 35

#define z_axis_2_claw_dir 51
#define z_axis_2_claw_step 50
#define z_axis_2_claw_enable 43

#define buzzer_pin 7
#define display_clock_pin 2
#define display_dio_pin 3




// ***************************************************
// **************** INITALISATION ********************
// ***************************************************
TM1637 digit_display(display_clock_pin, display_dio_pin);

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

AccelStepper x_axis_1_la_stepper(motorInterfaceType, x_axis_1_la_step, x_axis_1_la_dir);
AccelStepper x_axis_2_la_stepper(motorInterfaceType, x_axis_2_la_step, x_axis_2_la_dir);
AccelStepper z_axis_1_la_stepper(motorInterfaceType, z_axis_1_la_step, z_axis_1_la_dir);
AccelStepper z_axis_2_la_stepper(motorInterfaceType, z_axis_2_la_step, z_axis_2_la_dir);
AccelStepper y_axis_1_la_stepper(motorInterfaceType, y_axis_1_la_step, y_axis_1_la_dir);
AccelStepper y_axis_2_la_stepper(motorInterfaceType, y_axis_2_la_step, y_axis_2_la_dir);

AccelStepper x_axis_1_claw_stepper(motorInterfaceType, x_axis_1_claw_step, x_axis_1_claw_dir);
AccelStepper x_axis_2_claw_stepper(motorInterfaceType, x_axis_2_claw_step, x_axis_2_claw_dir);
AccelStepper z_axis_1_claw_stepper(motorInterfaceType, z_axis_1_claw_step, z_axis_1_claw_dir);
AccelStepper z_axis_2_claw_stepper(motorInterfaceType, z_axis_2_claw_step, z_axis_2_claw_dir);

LinearMotor x_axis_1_la = {&x_axis_1_la_stepper, x_axis_1_la_limit, x_axis_1_la_enable};
LinearMotor x_axis_2_la = {&x_axis_2_la_stepper, x_axis_2_la_limit, x_axis_2_la_enable};
LinearMotor z_axis_1_la = {&z_axis_1_la_stepper, z_axis_1_la_limit, z_axis_1_la_enable};
LinearMotor z_axis_2_la = {&z_axis_2_la_stepper, z_axis_2_la_limit, z_axis_2_la_enable};
LinearMotor y_axis_1_la = {&y_axis_1_la_stepper, y_axis_1_la_limit, y_axis_1_la_enable};
LinearMotor y_axis_2_la = {&y_axis_2_la_stepper, y_axis_2_la_limit, y_axis_2_la_enable};

ClawMotor x_axis_1_claw = {&x_axis_1_claw_stepper, x_axis_1_claw_enable};
ClawMotor x_axis_2_claw = {&x_axis_2_claw_stepper, x_axis_2_claw_enable};
ClawMotor z_axis_1_claw = {&z_axis_1_claw_stepper, z_axis_1_claw_enable};
ClawMotor z_axis_2_claw = {&z_axis_2_claw_stepper, z_axis_2_claw_enable};




// ***************************************************
// ****************** CONSTANTS **********************
// ***************************************************
std::array<LinearMotor, 6> la_motors = {x_axis_1_la, x_axis_2_la, z_axis_1_la, z_axis_2_la, y_axis_1_la, y_axis_2_la};
std::array<LinearMotor, 4> la_hub_motors = {x_axis_1_la, x_axis_2_la, z_axis_1_la, z_axis_2_la};
std::array<LinearMotor, 2> la_cg_motors = {y_axis_1_la, y_axis_2_la};
std::array<ClawMotor, 4> claw_motors = {x_axis_1_claw, x_axis_2_claw, z_axis_1_claw, z_axis_2_claw};

int steps_per_revolution = 200;
float pulley_circumfrence = 40.0;
float steps_per_mm = steps_per_revolution / pulley_circumfrence;

float linear_rail_distance = 62.2;
float depth_0_distance = 10.0;
float depth_1_distance = 33.6; 
float depth_2_distance = 47.2;
float depth_3_distance = 61.0;
float corner_grabber_engaged_distance = 56.5;
float corner_grabber_disengaged_distance = 42.0;

int depth_0_position = depth_0_distance * steps_per_mm;
int depth_1_position = depth_1_distance * steps_per_mm;
int depth_2_position = depth_2_distance * steps_per_mm;
int depth_3_position = depth_3_distance * steps_per_mm;
float corner_grabber_engaged_position = corner_grabber_engaged_distance * steps_per_mm;
float corner_grabber_disengaged_position = corner_grabber_disengaged_distance * steps_per_mm;

float linear_motor_max_speed = 40000.0;
float claw_motor_max_speed = 40000.0;
float linear_motor_acceleration = 8000.0;
float claw_motor_acceleration = 8000.0;





// ***************************************************
// ********************* SETUP ***********************
// ***************************************************
void setup() {
  Serial.begin(9600); 

  pinMode(x_axis_1_la_dir, OUTPUT); 
  pinMode(x_axis_1_la_step, OUTPUT); 
  pinMode(x_axis_2_la_dir, OUTPUT); 
  pinMode(x_axis_2_la_step, OUTPUT);
  pinMode(z_axis_1_la_dir, OUTPUT); 
  pinMode(z_axis_1_la_step, OUTPUT); 
  pinMode(z_axis_2_la_dir, OUTPUT); 
  pinMode(z_axis_2_la_step, OUTPUT); 
  pinMode(y_axis_1_la_dir, OUTPUT); 
  pinMode(y_axis_1_la_step, OUTPUT); 
  pinMode(y_axis_2_la_dir, OUTPUT); 
  pinMode(y_axis_2_la_step, OUTPUT); 
  pinMode(x_axis_1_claw_dir, OUTPUT); 
  pinMode(x_axis_1_claw_step, OUTPUT); 
  pinMode(x_axis_2_claw_dir, OUTPUT); 
  pinMode(x_axis_2_claw_step, OUTPUT); 
  pinMode(z_axis_1_claw_dir, OUTPUT); 
  pinMode(z_axis_1_claw_step, OUTPUT); 
  pinMode(z_axis_2_claw_dir, OUTPUT); 
  pinMode(z_axis_2_claw_step, OUTPUT); 
  
  pinMode(x_axis_1_la_enable, OUTPUT); 
  pinMode(x_axis_2_la_enable, OUTPUT); 
  pinMode(z_axis_1_la_enable, OUTPUT); 
  pinMode(z_axis_2_la_enable, OUTPUT); 
  pinMode(y_axis_1_la_enable, OUTPUT); 
  pinMode(y_axis_2_la_enable, OUTPUT); 
  pinMode(x_axis_1_claw_enable, OUTPUT); 
  pinMode(x_axis_2_claw_enable, OUTPUT); 
  pinMode(z_axis_1_claw_enable, OUTPUT); 
  pinMode(z_axis_2_claw_enable, OUTPUT); 
  pinMode(x_axis_1_la_limit, INPUT_PULLUP); 
  pinMode(x_axis_2_la_limit, INPUT_PULLUP); 
  pinMode(z_axis_1_la_limit, INPUT_PULLUP); 
  pinMode(z_axis_2_la_limit, INPUT_PULLUP); 
  pinMode(y_axis_1_la_limit, INPUT_PULLUP); 
  pinMode(y_axis_2_la_limit, INPUT_PULLUP); 
  pinMode(buzzer_pin, OUTPUT);
  
  digitalWrite(x_axis_1_la_enable, HIGH);
  digitalWrite(x_axis_2_la_enable, HIGH);
  digitalWrite(z_axis_1_la_enable, HIGH);
  digitalWrite(z_axis_2_la_enable, HIGH);
  digitalWrite(y_axis_1_la_enable, HIGH);
  digitalWrite(y_axis_2_la_enable, HIGH);
  digitalWrite(x_axis_1_claw_enable, HIGH);
  digitalWrite(x_axis_2_claw_enable, HIGH);
  digitalWrite(z_axis_1_claw_enable, HIGH);
  digitalWrite(z_axis_2_claw_enable, HIGH);

  for (LinearMotor &motor : la_motors) {
    motor.stepper->setMaxSpeed(linear_motor_max_speed);
    motor.stepper->setAcceleration(linear_motor_acceleration);
  }
  for (ClawMotor &motor : claw_motors) {
    motor.stepper->setMaxSpeed(claw_motor_max_speed);
    motor.stepper->setAcceleration(claw_motor_acceleration);
  }

  digit_display.init();
  digit_display.set(2);
  updateDisplay(micros(), micros());
  soundBuzzer();

}




// ***************************************************
// ******************* MAIN LOOP *********************
// ***************************************************
bool timer = false;
unsigned long start_time = micros(); 

void loop() {
  updateDisplay(start_time, micros());

  static String command = "";

  while (Serial.available() > 0) {
    char incoming_char = Serial.read();

    if (incoming_char == '\n' || incoming_char == '\r') {
      if (command.length() > 0) {
        apply_command(command);
        command = "";
      }
    } else {
      command += incoming_char;
    }
  }
}

void apply_command(String command) {
  if (command == "c") {
    calibrateLinearAcctuators(); 
  }
  else if (command == "C") {
    calibrateLinearAcctuatorsWithCube(); 
  }
  else if (command == "CY") {
    calibrateCornerGrabbers(); 
  }
  else if (command == "S") {
    move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  }
  else if (command == "N") {
    soundBuzzer(); 
  }
  else if (command =="T") {
    timer = !timer;
    soundBuzzer(); 
    start_time = micros(); 
  }
  else if (command == "S1") {
    float linear_motor_max_speed = 40000.0;
    float claw_motor_max_speed = 40000.0;
    float linear_motor_acceleration = 8000.0;
    float claw_motor_acceleration = 8000.0;
    for (LinearMotor &motor : la_motors) {
      motor.stepper->setMaxSpeed(linear_motor_max_speed);
      motor.stepper->setAcceleration(linear_motor_acceleration);
    }
    for (ClawMotor &motor : claw_motors) {
      motor.stepper->setMaxSpeed(claw_motor_max_speed);
      motor.stepper->setAcceleration(claw_motor_acceleration);
    }
  }
  else if (command == "S2") {
    float linear_motor_max_speed = 60000.0;
    float claw_motor_max_speed = 60000.0;
    float linear_motor_acceleration = 20000.0;
    float claw_motor_acceleration = 20000.0;
    for (LinearMotor &motor : la_motors) {
      motor.stepper->setMaxSpeed(linear_motor_max_speed);
      motor.stepper->setAcceleration(linear_motor_acceleration);
    }
    for (ClawMotor &motor : claw_motors) {
      motor.stepper->setMaxSpeed(claw_motor_max_speed);
      motor.stepper->setAcceleration(claw_motor_acceleration);
    }
  }
  else if (command == "S3") {
    float linear_motor_max_speed = 80000.0;
    float claw_motor_max_speed = 80000.0;
    float linear_motor_acceleration = 60000.0;
    float claw_motor_acceleration = 35000.0;
    for (LinearMotor &motor : la_motors) {
      motor.stepper->setMaxSpeed(linear_motor_max_speed);
      motor.stepper->setAcceleration(linear_motor_acceleration);
    }
    for (ClawMotor &motor : claw_motors) {
      motor.stepper->setMaxSpeed(claw_motor_max_speed);
      motor.stepper->setAcceleration(claw_motor_acceleration);
    }
  }
  else if (command == "SPIN") {
    Serial.println(command);
    enableClaws(x_axis_1_claw, x_axis_2_claw);
    enableClaws(z_axis_1_claw, z_axis_2_claw);
    x_axis_1_claw.stepper->move(200 * 30);
    x_axis_2_claw.stepper->move(200 * 30);
    z_axis_1_claw.stepper->move(200 * 30);
    z_axis_2_claw.stepper->move(200 * 30);
    while (x_axis_1_claw.stepper->distanceToGo() != 0 || x_axis_2_claw.stepper->distanceToGo() != 0 || z_axis_1_claw.stepper->distanceToGo() != 0 || z_axis_2_claw.stepper->distanceToGo() != 0) {
      x_axis_1_claw.stepper->run();
      x_axis_2_claw.stepper->run();
      z_axis_1_claw.stepper->run();
      z_axis_2_claw.stepper->run();
    }
    disableClaws(x_axis_1_claw, x_axis_2_claw);
    disableClaws(z_axis_1_claw, z_axis_2_claw);
    return;
  }
  else if (command == "A") {
    adjustCornerGrabbers();
  }
  else if (command == "0") {
    move2LinearHub(x_axis_2_la, x_axis_1_la, depth_0_position, depth_0_position);
    move2LinearHub(z_axis_2_la, z_axis_1_la, depth_0_position, depth_0_position);
  }
  else if (command == "00") {
    move2LinearHub(x_axis_2_la, x_axis_1_la, 0, 0);
    move2LinearHub(z_axis_2_la, z_axis_1_la, 0, 0);
  }
  else if (command == "1") {
    move2LinearHub(x_axis_2_la, x_axis_1_la, depth_0_position, depth_2_position);
  }
  else if (command == "2") {
    move2LinearHub(x_axis_2_la, x_axis_1_la, depth_2_position, depth_2_position);
  }
  else if (command == "3") {
    move2LinearHub(x_axis_2_la, x_axis_1_la, depth_3_position, depth_1_position);
  }
  else if (command == "4") {
    move2LinearHub(z_axis_1_la, z_axis_2_la, depth_0_position, depth_0_position);
  }
  else if (command == "5") {
    move2LinearHub(z_axis_1_la, z_axis_2_la, depth_1_position, depth_3_position);
  }
  else if (command == "6") {
    move2LinearHub(z_axis_1_la, z_axis_2_la, depth_2_position, depth_2_position);
  }
  else if (command == "7") {
    move2LinearHub(z_axis_1_la, z_axis_2_la, depth_3_position, depth_1_position);
  }
  else if (command == "8") {
    move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  }
  else if (command == "9") {
    move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  }
  else if (command != "") {
    apply_move(command);
  }
  Serial.println(command);
}



// ***************************************************
// **************** OTHER FUNCTIONS ******************
// ***************************************************
void soundBuzzer() {
  digitalWrite(buzzer_pin, HIGH);
  delay(100);
  digitalWrite(buzzer_pin, LOW);
}

void updateDisplay(unsigned long startTime, unsigned long endTime) {
  if (timer) {
    unsigned long durationMicroseconds = endTime - startTime;
    float durationMilliseconds = durationMicroseconds / 1000.0;
    float durationSeconds = durationMicroseconds / 1000000.0;
    int totalSeconds = (int)durationSeconds;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    int hundredths = ((int)durationMilliseconds % 1000) / 10;
    int8_t displayValues[4];
    if (totalSeconds >= 60) {
      displayValues[0] = static_cast<int8_t>(minutes / 10);
      displayValues[1] = static_cast<int8_t>(minutes % 10);
      displayValues[2] = static_cast<int8_t>(seconds / 10);
      displayValues[3] = static_cast<int8_t>(seconds % 10);
    } else {
      displayValues[0] = static_cast<int8_t>(totalSeconds / 10);
      displayValues[1] = static_cast<int8_t>(totalSeconds % 10);
      displayValues[2] = static_cast<int8_t>(hundredths / 10);
      displayValues[3] = static_cast<int8_t>(hundredths % 10);
    }
    digit_display.point(POINT_ON);
    digit_display.display(displayValues);
  }
}


void resetDisplay() {
  digit_display.clearDisplay();
  digit_display.point(POINT_OFF);
}

void calibrateLinearAcctuators() {
  for (LinearMotor &motor : la_motors) {
    digitalWrite(motor.enable_pin, LOW);
    while (digitalRead(motor.limit_pin) == HIGH) { 
      motor.stepper->move(-1); 
      motor.stepper->run();
      delay(10);
    }
    motor.stepper->stop();
    motor.stepper->setCurrentPosition(0);
    motor.stepper->moveTo(depth_0_position);
    motor.stepper->runToPosition();
    digitalWrite(motor.enable_pin, HIGH);
  }
  moveLinearHub(y_axis_2_la, corner_grabber_engaged_position);
}

void calibrateLinearAcctuatorsWithCube() {
  for (LinearMotor &motor : la_hub_motors) {
    digitalWrite(motor.enable_pin, LOW);
    while (digitalRead(motor.limit_pin) == HIGH) { 
      motor.stepper->move(-1); 
      motor.stepper->run();
      delay(10);
    }
    motor.stepper->stop();
    motor.stepper->setCurrentPosition(0);
    motor.stepper->moveTo(depth_0_position);
    motor.stepper->runToPosition();
    digitalWrite(motor.enable_pin, HIGH);
  }
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_2_position, depth_2_position);
  calibrateCornerGrabbers();
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_0_position, depth_0_position);
}

void calibrateCornerGrabbers() {
  for (LinearMotor &motor : la_cg_motors) {
    digitalWrite(motor.enable_pin, LOW);
    while (digitalRead(motor.limit_pin) == HIGH) { 
      motor.stepper->move(-1); 
      motor.stepper->run();
      delay(10);
    }
    motor.stepper->stop();
    motor.stepper->setCurrentPosition(0);
    digitalWrite(motor.enable_pin, HIGH);
  }
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
}

void moveLinearHub(LinearMotor motor, int position) {
  updateDisplay(start_time, micros());
  digitalWrite(motor.enable_pin, LOW);
  motor.stepper->moveTo(position);
  motor.stepper->runToPosition();
  digitalWrite(motor.enable_pin, HIGH);
}

void move2LinearHub(LinearMotor motor1, LinearMotor motor2, int position1, int position2) {
  updateDisplay(start_time, micros());
  digitalWrite(motor1.enable_pin, LOW);
  digitalWrite(motor2.enable_pin, LOW);
  motor1.stepper->moveTo(position1);
  motor2.stepper->moveTo(position2);
  while (motor1.stepper->distanceToGo() != 0 || motor2.stepper->distanceToGo() != 0) {
    motor1.stepper->run();
    motor2.stepper->run();
  }
  digitalWrite(motor1.enable_pin, HIGH);
  digitalWrite(motor2.enable_pin, HIGH);
}

void adjustCornerGrabbers() {
  digitalWrite(y_axis_1_la.enable_pin, LOW);
  digitalWrite(y_axis_2_la.enable_pin, LOW);
  y_axis_2_la.stepper->move(5);
  y_axis_2_la.stepper->runToPosition();
  digitalWrite(y_axis_1_la.enable_pin, HIGH);
  digitalWrite(y_axis_2_la.enable_pin, HIGH);
}

void enableClaws(ClawMotor claw1, ClawMotor claw2) {
  updateDisplay(start_time, micros());
  digitalWrite(claw1.enable_pin, LOW);
  digitalWrite(claw2.enable_pin, LOW);
}

void disableClaws(ClawMotor claw1, ClawMotor claw2) {
  updateDisplay(start_time, micros());
  digitalWrite(claw1.enable_pin, HIGH);
  digitalWrite(claw2.enable_pin, HIGH);
}

void rotateClaw(ClawMotor active_claw, int turns) {
  updateDisplay(start_time, micros());
  active_claw.stepper->move(50 * turns);
  while (active_claw.stepper->distanceToGo() != 0) {
    active_claw.stepper->run();
  }
}

void rotate2Claws(ClawMotor claw1, ClawMotor claw2, int turns) {
  updateDisplay(start_time, micros());
  claw1.stepper->move(50 * turns);
  claw2.stepper->move(50 * turns);
  while (claw1.stepper->distanceToGo() != 0 || claw2.stepper->distanceToGo() != 0) {
    claw1.stepper->run();
    claw2.stepper->run();
  }
}




// ***************************************************
// **************** MOVE FUNCTIONS *******************
// ***************************************************

void apply_move(String m) {
  if      (m == "R")   R_move(1); 
  else if (m == "R2")  R_move(2);
  else if (m == "R'")  R_move(-1);
  else if (m == "L")   L_move(1); 
  else if (m == "L2")  L_move(2);
  else if (m == "L'")  L_move(-1);
  else if (m == "F")   F_move(1); 
  else if (m == "F2")  F_move(2);
  else if (m == "F'")  F_move(-1);
  else if (m == "B")   B_move(1); 
  else if (m == "B2")  B_move(2);
  else if (m == "B'")  B_move(-1);
  else if (m == "U")   U_move(1); 
  else if (m == "U2")  U_move(2);
  else if (m == "U'")  U_move(-1);
  else if (m == "D")   D_move(1); 
  else if (m == "D2")  D_move(2);
  else if (m == "D'")  D_move(-1);
  else if (m == "r")   r_move(1); 
  else if (m == "r2")  r_move(2);
  else if (m == "r'")  r_move(-1);
  else if (m == "l")   l_move(1); 
  else if (m == "l2")  l_move(2);
  else if (m == "l'")  l_move(-1);
  else if (m == "f")   f_move(1); 
  else if (m == "f2")  f_move(2);
  else if (m == "f'")  f_move(-1);
  else if (m == "b")   b_move(1); 
  else if (m == "b2")  b_move(2);
  else if (m == "b'")  b_move(-1);
  else if (m == "u")   u_move(1); 
  else if (m == "u2")  u_move(2);
  else if (m == "u'")  u_move(-1);
  else if (m == "d")   d_move(1); 
  else if (m == "d2")  d_move(2);
  else if (m == "d'")  d_move(-1);
  else if (m == "Rw")  Rw_move(1); 
  else if (m == "Rw2") Rw_move(2);
  else if (m == "Rw'") Rw_move(-1);
  else if (m == "Lw")  Lw_move(1); 
  else if (m == "Lw2") Lw_move(2);
  else if (m == "Lw'") Lw_move(-1);
  else if (m == "Fw")  Fw_move(1); 
  else if (m == "Fw2") Fw_move(2);
  else if (m == "Fw'") Fw_move(-1);
  else if (m == "Bw")  Bw_move(1); 
  else if (m == "Bw2") Bw_move(2);
  else if (m == "Bw'") Bw_move(-1);
  else if (m == "Uw")  Uw_move(1); 
  else if (m == "Uw2") Uw_move(2);
  else if (m == "Uw'") Uw_move(-1);
  else if (m == "Dw")  Dw_move(1); 
  else if (m == "Dw2") Dw_move(2);
  else if (m == "Dw'") Dw_move(-1);
}

void R_move(int turns) {
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_3_position, depth_1_position);
  enableClaws(x_axis_2_claw, x_axis_1_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotateClaw(x_axis_2_claw, turns);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  adjustCornerGrabbers();
  disableClaws(x_axis_2_claw, x_axis_1_claw);
}

void L_move(int turns) {
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_1_position, depth_3_position);
  enableClaws(x_axis_1_claw, x_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotateClaw(x_axis_1_claw, -turns);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  adjustCornerGrabbers();
  disableClaws(x_axis_1_claw, x_axis_2_claw);
}

void F_move(int turns) {
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_1_position, depth_3_position);
  enableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotateClaw(z_axis_1_claw, turns);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  adjustCornerGrabbers();
  disableClaws(z_axis_1_claw, z_axis_2_claw);
}

void B_move(int turns) {
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_3_position, depth_1_position);
  enableClaws(z_axis_2_claw, z_axis_1_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotateClaw(z_axis_2_claw, -turns);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  adjustCornerGrabbers();
  disableClaws(z_axis_2_claw, z_axis_1_claw);
}

void U_move(int turns) {
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_2_position, depth_2_position);
  enableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotate2Claws(z_axis_1_claw, z_axis_2_claw, 1);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  disableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_0_position, depth_0_position);
  R_move(turns);
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_2_position, depth_2_position);
  enableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotate2Claws(z_axis_1_claw, z_axis_2_claw, -1);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  adjustCornerGrabbers();
  disableClaws(z_axis_1_claw, z_axis_2_claw);
}

void D_move(int turns) {
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_2_position, depth_2_position);
  enableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotate2Claws(z_axis_1_claw, z_axis_2_claw, -1);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  disableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_0_position, depth_0_position);
  R_move(turns);
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_2_position, depth_2_position);
  enableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotate2Claws(z_axis_1_claw, z_axis_2_claw, 1);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  adjustCornerGrabbers();
  disableClaws(z_axis_1_claw, z_axis_2_claw);
}

void r_move(int turns) {
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_2_position, depth_2_position);
  enableClaws(x_axis_1_claw, x_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotateClaw(x_axis_2_claw, turns);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_3_position, depth_1_position);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotateClaw(x_axis_2_claw, -turns);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  adjustCornerGrabbers();
  disableClaws(x_axis_1_claw, x_axis_2_claw);
}

void l_move(int turns) {
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_2_position, depth_2_position);
  enableClaws(x_axis_1_claw, x_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotateClaw(x_axis_1_claw, -turns);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_1_position, depth_3_position);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotateClaw(x_axis_1_claw, turns);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  adjustCornerGrabbers();
  disableClaws(x_axis_1_claw, x_axis_2_claw);
}

void f_move(int turns) {
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_2_position, depth_2_position);
  enableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotateClaw(z_axis_1_claw, turns);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_1_position, depth_3_position);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotateClaw(z_axis_1_claw, -turns);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  adjustCornerGrabbers();
  disableClaws(z_axis_1_claw, z_axis_2_claw);
}

void b_move(int turns) {
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(z_axis_2_la, z_axis_1_la, depth_2_position, depth_2_position);
  enableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotateClaw(z_axis_2_claw, -turns);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  move2LinearHub(z_axis_2_la, z_axis_1_la, depth_1_position, depth_3_position);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotateClaw(z_axis_2_claw, turns);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  adjustCornerGrabbers();
  disableClaws(z_axis_1_claw, z_axis_2_claw);
}

void u_move(int turns) {
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_2_position, depth_2_position);
  enableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotate2Claws(z_axis_1_claw, z_axis_2_claw, 1);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  disableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_0_position, depth_0_position);
  r_move(turns);
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_2_position, depth_2_position);
  enableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotate2Claws(z_axis_1_claw, z_axis_2_claw, -1);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  adjustCornerGrabbers();
  disableClaws(z_axis_1_claw, z_axis_2_claw);
}

void d_move(int turns) {
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_2_position, depth_2_position);
  enableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotate2Claws(z_axis_1_claw, z_axis_2_claw, -1);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  disableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_0_position, depth_0_position);
  r_move(turns);
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_2_position, depth_2_position);
  enableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotate2Claws(z_axis_1_claw, z_axis_2_claw, 1);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  adjustCornerGrabbers();
  disableClaws(z_axis_1_claw, z_axis_2_claw);
}

void Rw_move(int turns) {
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_2_position, depth_2_position);
  enableClaws(x_axis_1_claw, x_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotateClaw(x_axis_2_claw, turns);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  adjustCornerGrabbers();
  disableClaws(x_axis_1_claw, x_axis_2_claw);
}

void Lw_move(int turns) {
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_2_position, depth_2_position);
  enableClaws(x_axis_1_claw, x_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotateClaw(x_axis_1_claw, -turns);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  adjustCornerGrabbers();
  disableClaws(x_axis_1_claw, x_axis_2_claw);
}

void Fw_move(int turns) {
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_2_position, depth_2_position);
  enableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotateClaw(z_axis_1_claw, turns);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  adjustCornerGrabbers();
  disableClaws(z_axis_1_claw, z_axis_2_claw);
}

void Bw_move(int turns) {
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_2_position, depth_2_position);
  enableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotateClaw(z_axis_2_claw, -turns);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  adjustCornerGrabbers();
  disableClaws(z_axis_1_claw, z_axis_2_claw);
}

void Uw_move(int turns) {
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_2_position, depth_2_position);
  enableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotate2Claws(z_axis_1_claw, z_axis_2_claw, 1);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  disableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_0_position, depth_0_position);
  Rw_move(turns);
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_2_position, depth_2_position);
  enableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotate2Claws(z_axis_1_claw, z_axis_2_claw, -1);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  adjustCornerGrabbers();
  disableClaws(z_axis_1_claw, z_axis_2_claw);
}

void Dw_move(int turns) {
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_2_position, depth_2_position);
  enableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotate2Claws(z_axis_1_claw, z_axis_2_claw, -1);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  disableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_0_position, depth_0_position);
  Rw_move(turns);
  move2LinearHub(x_axis_1_la, x_axis_2_la, depth_0_position, depth_0_position);
  move2LinearHub(z_axis_1_la, z_axis_2_la, depth_2_position, depth_2_position);
  enableClaws(z_axis_1_claw, z_axis_2_claw);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_disengaged_position, corner_grabber_disengaged_position);
  rotate2Claws(z_axis_1_claw, z_axis_2_claw, 1);
  move2LinearHub(y_axis_1_la, y_axis_2_la, corner_grabber_engaged_position, corner_grabber_engaged_position);
  adjustCornerGrabbers();
  disableClaws(z_axis_1_claw, z_axis_2_claw);
}

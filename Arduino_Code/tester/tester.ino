/*
 * Double check before running:
 *  cutterPin should be 10 if using the built in RAMPS MOSFET, and 16 (or other) if using an auxiliary transistor
 *  The Z axis moves starting at 0 (home) and moves down, so DON'T USE POSITIVE NUMBERS. It will go up.
 *  
 * To Do:
 *  Limit detection while running
 *  Cutter mechanism control using onboard RAMPS MOSFET
 *  Cutter constants (offset to get to cutter)
 *  Other constants (X Y and Z max)
*/

enum actions {HOME_AXIS, SET_HOME, GO_TO_LOCATION, GO_TO_HOME, GO_TO_CUTTER, CUT, HALT};

struct location {
  double xCoordinate;
  double yCoordinate;
  double zCoordinate;
  double eCoordinate;
};

struct command {
  actions operation;
  location targetLocation;
};

#include "A4988.h"
#include "MultiDriver.h"
#include "SyncDriver.h"

#define X_MOTOR_STEPS 200
#define X_RPM 200
#define X_MICROSTEPS 1
#define X_DIR 55
#define X_STEP 54
#define X_ENABLE 38
#define X_SWITCH 2
#define X_MS1 10
#define X_MS2 11
#define X_MS3 12
#define X_REVS_PER_MM 3.33

#define Y_MOTOR_STEPS 200
#define Y_RPM 200
#define Y_MICROSTEPS 1
#define Y_DIR 48
#define Y_STEP 46
#define Y_ENABLE 62
#define Y_SWITCH 3
#define Y_MS1 10
#define Y_MS2 11
#define Y_MS3 12
#define Y_REVS_PER_MM 5.0

#define Z_MOTOR_STEPS 200
#define Z_RPM 150
#define Z_MICROSTEPS 1
#define Z_DIR 34
#define Z_STEP 36
#define Z_ENABLE 30
#define Z_SWITCH 18
#define Z_MS1 10
#define Z_MS2 11
#define Z_MS3 12
#define Z_REVS_PER_MM 100

#define E_MOTOR_STEPS 200
#define E_RPM 150
#define E_MICROSTEPS 1
#define E_DIR 28
#define E_STEP 26
#define E_ENABLE 24
#define E_MS1 10
#define E_MS2 11
#define E_MS3 12
#define E_REVS_PER_MM 5.7874574

const int X_ACCEL = 667;
const int X_DECEL = 667;

const int Y_ACCEL = 1000;
const int Y_DECEL = 1000;

const int Z_ACCEL = 1000;
const int Z_DECEL = 1000;

const int E_ACCEL = 1000;
const int E_DECEL = 1000;

const int cutterPin = 16;
const int cutterDelay = 200;

bool xComplete = false;
bool yComplete = false;
bool zComplete = false;
bool eComplete = false;

double xPosition;
double yPosition;
double zPosition;
double ePosition;

volatile bool homing = false;

A4988 xMotor(X_MOTOR_STEPS, X_DIR, X_STEP, X_ENABLE, X_MS1, X_MS2, X_MS3);
A4988 yMotor(Y_MOTOR_STEPS, Y_DIR, Y_STEP, Y_ENABLE, Y_MS1, Y_MS2, Y_MS3);
A4988 zMotor(Z_MOTOR_STEPS, Z_DIR, Z_STEP, Z_ENABLE, Z_MS1, Z_MS2, Z_MS3);
A4988 eMotor(E_MOTOR_STEPS, E_DIR, E_STEP, E_ENABLE, E_MS1, E_MS2, E_MS3);

MultiDriver xyController(xMotor, yMotor);
MultiDriver zeController(zMotor, eMotor);

command currentCommand;
location currentLocation = {0.0, 0.0, 0.0, 0.0};
location cutter = {0.0, 0.0, 0.0, 0.0};
location cameraInView = {0.0, 0.0, 0.0, 0.0};
location home = {0.0, 0.0, 0.0, 0.0};
bool done = false;
int homeDelay = 0;

void xAxisSwitch() {
  if (homing) {
    xMotor.stop();
  }
}

void yAxisSwitch() {
  if (homing) {
    yMotor.stop();
  }
}

void zAxisSwitch() {
  if (homing) {
    zMotor.stop();
  }
}

void setup() {
  Serial.begin(9600);
  xMotor.begin(X_RPM, X_MICROSTEPS);
  yMotor.begin(Y_RPM, Y_MICROSTEPS);
  zMotor.begin(Z_RPM, Z_MICROSTEPS);
  xMotor.enable();
  xMotor.setSpeedProfile(xMotor.LINEAR_SPEED, X_ACCEL, X_DECEL);
  yMotor.enable();
  yMotor.setSpeedProfile(yMotor.LINEAR_SPEED, Y_ACCEL, Y_DECEL);
  zMotor.enable();
  zMotor.setSpeedProfile(zMotor.LINEAR_SPEED, Z_ACCEL, Z_DECEL);
  eMotor.enable();
  eMotor.setSpeedProfile(eMotor.LINEAR_SPEED, E_ACCEL, E_DECEL);
  pinMode(X_SWITCH, INPUT_PULLUP); attachInterrupt(digitalPinToInterrupt(X_SWITCH), xAxisSwitch, FALLING);
  pinMode(Y_SWITCH, INPUT_PULLUP); attachInterrupt(digitalPinToInterrupt(Y_SWITCH), yAxisSwitch, FALLING);
  pinMode(Z_SWITCH, INPUT_PULLUP); attachInterrupt(digitalPinToInterrupt(Z_SWITCH), zAxisSwitch, FALLING);
  pinMode(cutterPin, OUTPUT);
  digitalWrite(cutterPin, LOW);
}

void loop() {
  if (Serial.available()) {
    getCommand();
    done = false;
  }
  if (!done && !xyController.isRunning() && !zeController.isRunning()) {
    done = true;
  }
  if (done) {
    sendDoneSignal();
  }
}

void getCommand() {
  currentCommand.operation = Serial.parseInt();
  switch (currentCommand.operation) {
    case HOME_AXIS:
      homeAxis(Serial.parseInt());
      break;
    case SET_HOME:
      currentLocation.xCoordinate = 0;
      currentLocation.yCoordinate = 0;
      currentLocation.zCoordinate = 0;
      currentLocation.eCoordinate = 0;
      break;
    case GO_TO_LOCATION:
      currentCommand.targetLocation.xCoordinate = Serial.parseFloat();
      currentCommand.targetLocation.yCoordinate = Serial.parseFloat();
      currentCommand.targetLocation.zCoordinate = Serial.parseFloat();
      currentCommand.targetLocation.eCoordinate = Serial.parseFloat();
      break;
    case GO_TO_HOME:
      currentCommand.targetLocation = home;
      break;
    case GO_TO_CUTTER:
      currentCommand.targetLocation = cutter;
      break;
    case CUT:
      cut();
      break;
    case HALT:
      currentCommand.targetLocation = currentLocation;
      break;
  }
  moveCarrige();
  moveEffector();
  currentLocation.xCoordinate = currentCommand.targetLocation.xCoordinate;
  currentLocation.yCoordinate = currentCommand.targetLocation.yCoordinate;
  currentLocation.zCoordinate = currentCommand.targetLocation.zCoordinate;
  currentLocation.eCoordinate = currentCommand.targetLocation.eCoordinate;
}

void sendDoneSignal() {
  Serial.print("X:");
  Serial.print(currentLocation.xCoordinate);
  Serial.print(", ");
  Serial.print("Y:");
  Serial.print(currentLocation.yCoordinate);
  Serial.print(", ");
  Serial.print("Z:");
  Serial.print(currentLocation.zCoordinate);
  Serial.print(", ");
  Serial.print("E:");
  Serial.println(currentLocation.eCoordinate);
}

void moveCarrige() {
  xyController.rotate(mmToRevs(currentCommand.targetLocation.xCoordinate, 'x') - mmToRevs(currentLocation.xCoordinate, 'x'), mmToRevs(currentCommand.targetLocation.yCoordinate, 'y') - mmToRevs(currentLocation.yCoordinate, 'y'));
}

void moveEffector() {
  zeController.rotate(mmToRevs(currentCommand.targetLocation.zCoordinate, 'z') - mmToRevs(currentLocation.zCoordinate, 'z'), mmToRevs(currentCommand.targetLocation.eCoordinate, 'e') - mmToRevs(currentLocation.eCoordinate, 'e'));
}

double mmToRevs(double mm, char axis) {
  switch (axis) {
    case 'x':
      return mm * X_REVS_PER_MM;
    case 'y':
      return mm * X_REVS_PER_MM;
    case 'z':
      return mm * X_REVS_PER_MM;
    case 'e':
      return mm * X_REVS_PER_MM;
  }
}

void cut() {
  digitalWrite(cutterPin, HIGH);
  delay(cutterDelay);
  digitalWrite(cutterPin, LOW);
}

void homeAxis(int axis) {
  homing = true;
  switch (axis) {
    case 0:
      xMotor.rotate(-5000);
      homing = false;
      currentLocation.xCoordinate = 0;
      break;
    case 1:
      yMotor.rotate(-5000);
      homing = false;
      currentLocation.yCoordinate = 0;
      break;
    case 2:
      zMotor.rotate(5000);
      homing = false;
      currentLocation.zCoordinate = 0;
      break;
    default: {
        Serial.println("That's not an axis!");
        break;
      }
  }
}

/*
  TODO:
  Add speed changing?
  Test everything
*/

//Xmax = 75000
//Ymax = 60000
//Zmax = 6500
//Emax = 

enum actions {HOME, MOVE_FOR_CAMERA, PICKUP_COLONY, PLACE_COLONY, CUT_COLONY, HALT};

struct location
{
  double xCoordinate;
  double yCoordinate;
  double zCoordinate;
  double eCoordinate;
};

struct command
{
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
#define X_MS1 10
#define X_MS2 11
#define X_MS3 12
#define X_REVS_PER_MM 0.03125

#define Y_MOTOR_STEPS 200
#define Y_RPM 150
#define Y_MICROSTEPS 1
#define Y_DIR 48
#define Y_STEP 46
#define Y_ENABLE 62
#define Y_MS1 10
#define Y_MS2 11
#define Y_MS3 12
#define Y_REVS_PER_MM 0.03125

#define Z_MOTOR_STEPS 200
#define Z_RPM 150
#define Z_MICROSTEPS 1
#define Z_DIR 34
#define Z_STEP 36
#define Z_ENABLE 30
#define Z_MS1 10
#define Z_MS2 11
#define Z_MS3 12
#define Z_REVS_PER_MM 0.5

#define E_MOTOR_STEPS 200
#define E_RPM 150
#define E_MICROSTEPS 1
#define E_DIR 28
#define E_STEP 26
#define E_ENABLE 24
#define E_MS1 10
#define E_MS2 11
#define E_MS3 12
#define E_REVS_PER_MM 0.028937287

const int X_ACCEL = 1000;
const int X_DECEL = 1000;

const int Y_ACCEL = 1000;
const int Y_DECEL = 1000;

const int Z_ACCEL = 1000;
const int Z_DECEL = 1000;

const int E_ACCEL = 1000;
const int E_DECEL = 1000;

bool xComplete = false;
bool yComplete = false;
bool zComplete = false;
bool eComplete = false;

double xPosition;
double yPosition;
double zPosition;
double ePosition;

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

void setup() {
  Serial.begin(9600);
  xMotor.begin(X_RPM, X_MICROSTEPS);
  yMotor.begin(Y_RPM, Y_MICROSTEPS);
  zMotor.begin(Z_RPM, Z_MICROSTEPS);
  eMotor.begin(E_RPM, E_MICROSTEPS);
  xMotor.enable();
  xMotor.setSpeedProfile(xMotor.LINEAR_SPEED, X_ACCEL, X_DECEL);
  yMotor.enable();
  yMotor.setSpeedProfile(yMotor.LINEAR_SPEED, Y_ACCEL, Y_DECEL);
  zMotor.enable();
  zMotor.setSpeedProfile(zMotor.LINEAR_SPEED, Z_ACCEL, Z_DECEL);
  eMotor.enable();
  eMotor.setSpeedProfile(eMotor.LINEAR_SPEED, E_ACCEL, E_DECEL);
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
    case HOME:
      currentCommand.targetLocation = home;
      break;
    case MOVE_FOR_CAMERA:
      currentCommand.targetLocation = cameraInView;
      break;
    case PICKUP_COLONY:
      currentCommand.targetLocation.xCoordinate = Serial.parseFloat();
      currentCommand.targetLocation.yCoordinate = Serial.parseFloat();
      currentCommand.targetLocation.zCoordinate = Serial.parseFloat();
      currentCommand.targetLocation.eCoordinate = Serial.parseFloat();
      break;
    case PLACE_COLONY:
      currentCommand.targetLocation.xCoordinate = Serial.parseFloat();
      currentCommand.targetLocation.yCoordinate = Serial.parseFloat();
      currentCommand.targetLocation.zCoordinate = Serial.parseFloat();
      currentCommand.targetLocation.eCoordinate = Serial.parseFloat();
      break;
    case CUT_COLONY:
      currentCommand.targetLocation = cutter;
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
  xyController.rotate(mmToRevs(currentCommand.targetLocation.xCoordinate - currentLocation.xCoordinate, 'x'), mmToRevs(currentCommand.targetLocation.yCoordinate - currentLocation.yCoordinate, 'y'));
}

void moveEffector() {
  zeController.rotate(mmToRevs(currentCommand.targetLocation.zCoordinate - currentLocation.zCoordinate, 'z'), mmToRevs(currentCommand.targetLocation.eCoordinate - currentLocation.eCoordinate, 'e'));
}

double mmToRevs(double mm, char axis) {
  if(axis == 'x') {
    return mm * X_REVS_PER_MM;
  }
  if(axis == 'y') {
    return mm * Y_REVS_PER_MM;
  }
  if(axis == 'z') {
    return mm * Z_REVS_PER_MM;
  }
  if(axis == 'e') {
    return mm * E_REVS_PER_MM;
  }
}


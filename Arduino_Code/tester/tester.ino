/*
TODO:
Add serial reception
Add command-based motor starting
Add speed changing?
Test everything
*/

enum actions {HOME, MOVE_FOR_CAMERA, PICKUP_COLONY, PLACE_COLONY, CUT_COLONY, HALT};

typedef struct location
{
	double xCoordinate;
	double yCoordinate;
}

typedef struct command
{
	actions operation;	
	location targetLocation;
}

#include "A4988.h"

#define X_MOTOR_STEPS 200
#define X_RPM 100
#define X_MICROSTEPS 1
#define X_DIR 55
#define X_STEP 54
#define X_ENABLE 38
#define X_MS1 10
#define X_MS2 11
#define X_MS3 12
#define X_STEPS_MM 100

#define Y_MOTOR_STEPS 200
#define Y_RPM 100
#define Y_MICROSTEPS 1
#define Y_DIR 55
#define Y_STEP 54
#define Y_ENABLE 38
#define Y_MS1 10
#define Y_MS2 11
#define Y_MS3 12
#define Y_STEPS_MM 100

#define Z_MOTOR_STEPS 200
#define Z_RPM 100
#define Z_MICROSTEPS 1
#define Z_DIR 55
#define Z_STEP 54
#define Z_ENABLE 38
#define Z_MS1 10
#define Z_MS2 11
#define Z_MS3 12
#define Z_STEPS_MM 100

#define E_MOTOR_STEPS 200
#define E_RPM 100
#define E_MICROSTEPS 1
#define E_DIR 55
#define E_STEP 54
#define E_ENABLE 38
#define E_MS1 10
#define E_MS2 11
#define E_MS3 12
#define E_STEPS_MM 100

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

A4988 xMotor(X_STEPS, X_DIR, X_STEP, X_ENABLE, X_MS1, X_MS2, X_MS3);
A4988 yMotor(Y_STEPS, Y_DIR, Y_STEP, Y_ENABLE, Y_MS1, Y_MS2, Y_MS3);
A4988 zMotor(Z_STEPS, Z_DIR, Z_STEP, Z_ENABLE, Z_MS1, Z_MS2, Z_MS3);
A4988 eMotor(E_STEPS, E_DIR, E_STEP, E_ENABLE, E_MS1, E_MS2, E_MS3);

command currentCommand;
location currentLocation;

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
 	if(Serial.available()) {
		getCommand(&currentCommand);
	}
	if(!xMotor.nextAction()) {
		xComplete = true;
	}
 	if(!yMotor.nextAction()) {
		yComplete = true;
	}
 	if(!zMotor.nextAction()) {
		zComplete = true;
	}
 	if(!eMotor.nextAction()) {
		eComplete = true;
	}
	
}

void getCommand(command *c) {
	c->action = Serial.read();
}
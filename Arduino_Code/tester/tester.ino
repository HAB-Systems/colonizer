#define MOTOR_STEPS 200
#define RPM 100

#define DIR 55
#define STEP 54
#define ENABLE 38

int MOTOR_ACCEL = 1000;
int MOTOR_DECEL = 1000;
#define MICROSTEPS 1

#include "A4988.h"
#define MS1 10
#define MS2 11
#define MS3 12
A4988 stepper(MOTOR_STEPS, DIR, STEP, ENABLE, MS1, MS2, MS3);

void setup() {
    stepper.begin(RPM, MICROSTEPS);
    stepper.enable();
    stepper.setSpeedProfile(stepper.LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);
}

void loop() {
  if(!stepper.nextAction()) {
    stepToIt();
  }
}

void stepToIt() {
  stepper.startRotate(360);
  delay(500);
}


/*
TODO
Add endpoint sensors
Add more advanced control structure
Add serial comms
Implement data structure
*/

#include <Stepper.h>

enum state{Standby, Running, Done} myState;

const int stepsPerRevoloution = 200;
const double stepsPerMMX = 10;
const double stepsPerMMY = 10;

Stepper xAxis(stepsPerRevoloution, 3, 4, 5, 6);
Stepper yAxis(stepsPerRevoloution, 7, 8, 9, 10);

void setup() {
  // put your setup code here, to run once:
  
}

void loop() {
  // put your main code here, to run repeatedly:
  switch (myState) {
    case Running:
      break;
    case Done:
      break;
    default:
      break;
  }
}

double stepsToMM(int steps, int axis) {
  return axis == 0 ? steps / stepsPerMMX : steps / stepsPerMMY;
}

double mmToSteps(double distance, int axis) {
  return axis == 0 ? distance * stepsPerMMX : distance * stepsPerMMY;
}

void homeXAxis() {
  
}

void homeYAxis() {
  
}

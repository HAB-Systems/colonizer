#include "A4988.h"
#include "MultiDriver.h"
#include "SyncDriver.h"

#define X_SWITCH 2
#define Y_SWITCH 3
#define Z_SWITCH 18

volatile bool xHome;
volatile bool yHome;
volatile bool zHome;

void xAxisSwitch() {
  Serial.print("x");
}

void yAxisSwitch() {
  Serial.print("y");
}

void zAxisSwitch() {
  Serial.print("z");
}

void setup() {
  Serial.begin(9600);
  pinMode(X_SWITCH, INPUT_PULLUP); attachInterrupt(digitalPinToInterrupt(X_SWITCH), xAxisSwitch, FALLING);
  pinMode(Y_SWITCH, INPUT_PULLUP); attachInterrupt(digitalPinToInterrupt(Y_SWITCH), yAxisSwitch, FALLING);
  pinMode(Z_SWITCH, INPUT_PULLUP); attachInterrupt(digitalPinToInterrupt(Z_SWITCH), zAxisSwitch, FALLING);
}

void loop() {
  
}

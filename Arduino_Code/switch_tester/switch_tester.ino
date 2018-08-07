#include "A4988.h"
#include "MultiDriver.h"
#include "SyncDriver.h"

#define X_SWITCH 2
#define Y_SWITCH 3
#define Z_SWITCH 18
#define E_SWITCH 19

volatile bool xHome;
volatile bool yHome;
volatile bool zHome;
volatile bool eHome;

void xAxisSwitch() {
  xHome = true;
}

void yAxisSwitch() {
  yHome = true;
}

void zAxisSwitch() {
  zHome = true;
}

void eAxisSwitch() {
  eHome = true;
}

void setup() {
  pinMode(X_SWITCH, INPUT_PULLUP); attachInterrupt(digitalPinToInterrupt(X_SWITCH), xAxisSwitch, FALLING);
  pinMode(Y_SWITCH, INPUT_PULLUP); attachInterrupt(digitalPinToInterrupt(Y_SWITCH), yAxisSwitch, FALLING);
  pinMode(Z_SWITCH, INPUT_PULLUP); attachInterrupt(digitalPinToInterrupt(Z_SWITCH), zAxisSwitch, FALLING);
  pinMode(E_SWITCH, INPUT_PULLUP); attachInterrupt(digitalPinToInterrupt(E_SWITCH), eAxisSwitch, FALLING);
}

void loop() {
  if(xHome) {
    Serial.print("x");
  }
  if(yHome) {
    Serial.print("y");
  }
  if(zHome) {
    Serial.print("z");
  }
  if(eHome) {
    Serial.print("e");
  }
  Serial.println();
}

#ifndef LINE_H_
#define LINE_H_

#include <Arduino.h>
#include <RBE1001Lib.h>
#include "Chassis.h"

const unsigned long LINE_FOLLOWING_INTERVAL = 10;
unsigned long lastTime = 0;
float kp = 0.04;
int threshhold = 1500; // value for line sensor array, >= denotes a line

void followLine(float baseSpeed);
bool checkForIntersection();
bool checkNewLine(bool right);

#endif
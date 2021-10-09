#pragma once

#include <Arduino.h>
#include <RBE1001Lib.h>
#include "Chassis.h"

class Line {
    public:
        void setup();
        void followLine(float baseSpeed, Motor left_motor, Motor right_motor);
        bool checkForIntersection();
        bool checkNewLine(bool right);
    
    private:
        const unsigned long LINE_FOLLOWING_INTERVAL = 10;
        unsigned long lastTime = 0;
        float kp = 0.04;
        int threshold = 1500; // value for line sensor array, >= denotes a line
};
#pragma once

#include <Arduino.h>
#include <RBE1001Lib.h>
#include <ESP32AnalogRead.h>

class Line {
    public:
        void setup();
        void followLine(float baseSpeed, LeftMotor* left_motor, RightMotor* right_motor);
        bool checkForIntersection();
        bool checkNewLine(bool right);
    
    private:
        const unsigned long LINE_FOLLOWING_INTERVAL = 10;
        float kp = 0.04;
        float ki = 0.003;
        int threshold = 1500; // value for line sensor array, >= denotes a line
};
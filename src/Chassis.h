#pragma once

// Class for the Chassis and everything on it 
// (IR Detector, Drivetrain, Ultrasonic, BootButton)

#include <Arduino.h>
#include <RBE1001Lib.h>
#include <ESP32AnalogRead.h>
#include <math.h>

#include <button.h>
#include "line.h"

class Chassis {
    public:
        float robotSpeed = 80; // Speed of the robot in ticks/s
        
        void setup();
        bool moveTo(float distInCm);
        void setSpeed(float speed);
        void turn(float speed, bool right);
        // void moveFor(float speedInCmPerS);
        void followPath(bool turnRightAtIntersection);
        bool turnToLine(bool turnRight);
        void stop();

    private:
        const float WHEEL_DIAMETER = 2.75; // Wheel diameter in inches
        const float WHEEL_TRACK = 15; // Distance between wheels in cm
        const float SENSOR_DISTANCE = 8; // Distance between center of wheel and line sensor in cm        

        LeftMotor left_motor;
        RightMotor right_motor;

        int pathState = 0; // 0 for line following, 1 for turning at intersection
        long startTime = 0; // ms
        const long MAX_TURN_TIME = 1000; // ms
};
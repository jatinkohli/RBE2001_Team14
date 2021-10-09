#pragma once

// Class for the Chassis and everything on it 
// (IR Detector, Drivetrain, Ultrasonic, BootButton)

#include <Arduino.h>
#include <RBE1001Lib.h>
#include <ESP32AnalogRead.h>
#include <math.h>

#include <button.h>
#include <IRdecoder.h>
#include "line.h"

class Chassis {
    public:
        void setup();
        void moveTo(float distInCm);
        void moveFor(float speedInCmPerS);
        void followPath(bool turnRightAtIntersection);
        void turnToLine(bool turnRight);
        void stop();

    private:
        const float WHEEL_DIAMETER = 2.75; // Wheel diameter in inches
        const float WHEEL_TRACK = 15; // Distance between wheels in cm
        const float SENSOR_DISTANCE = 8; // Distance between center of wheel and line sensor in cm

        LeftMotor left_motor;
        RightMotor right_motor;
};
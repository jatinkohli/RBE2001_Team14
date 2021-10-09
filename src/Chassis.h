// #pragma once

// // Class for the Chassis and everything on it 
// // (IR Detector, Drivetrain, Ultrasonic, BootButton)

// #include <Arduino.h>
// #include <RBE1001Lib.h>
// #include <ESP32AnalogRead.h>
// #include <math.h>

// #include <button.h>
// #include <IRdecoder.h>
// #include "line.h"

// class Chassis {
//     public:
//         void setup();
//         void moveTo(float distInCm);
//         void followPath(bool turnRightAtIntersection);
//         void turnToLine(bool turnRight);
//         void stop();

//     private:
//         const uint8_t IR_DETECTOR_PIN = 15; // define the pin for the IR receiver
//         const float WHEEL_DIAMETER = 2.75; // Wheel diameter in inches
//         const float WHEEL_TRACK = 15; // Distance between wheels in cm
//         const float SENSOR_DISTANCE = 8; // Distance between center of wheel and line sensor in cm
//         float robotSpeed = 180; // Speed of the robot in ticks/s

//         Rangefinder ultrasonic;
//         IRDecoder decoder(IR_DETECTOR_PIN); // create an IRDecoder object
//         Motor left_motor(MOTOR_LEFT_PWM, MOTOR_LEFT_DIR, MOTOR_LEFT_ENCA, MOTOR_LEFT_ENCB);
//         Motor right_motor(MOTOR_RIGHT_PWM, MOTOR_RIGHT_DIR, MOTOR_RIGHT_ENCA, MOTOR_RIGHT_ENCB);
//         Button bootButton(BOOT_FLAG_PIN); // Create a button object for the built-in button on the ESP32

//         Line line;
// };
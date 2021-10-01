#ifndef CHASSIS_H_
#define CHASSIS_H_

// Class for the Chassis and everything on it 
// (IR Detector, Drivetrain, Ultrasonic, BootButton)

#include <Arduino.h>
#include <RBE1001Lib.h>
#include <button.h>
#include <IRdecoder.h>
#include <ESP32Servo.h>
#include <ESP32AnalogRead.h>
#include <math.h>

// define the pin for the IR receiver
const uint8_t IR_DETECTOR_PIN = 15;
// Wheel diameter in inches
const float WHEEL_DIAMETER = 2.75;
// Distance between wheels in cm
const float WHEEL_TRACK = 15;
// Distance between center of wheel and line sensor in cm
const float SENSOR_DISTANCE = 8;
// Speed of the robot in ticks/s
float robotSpeed = 180;

Rangefinder ultrasonic;
IRDecoder decoder(IR_DETECTOR_PIN); // create an IRDecoder object
Motor left_motor(MOTOR_LEFT_PWM, MOTOR_LEFT_DIR, MOTOR_LEFT_ENCA, MOTOR_LEFT_ENCB);
Motor right_motor(MOTOR_RIGHT_PWM, MOTOR_RIGHT_DIR, MOTOR_RIGHT_ENCA, MOTOR_RIGHT_ENCB);

// Create a button object for the built-in button on the ESP32
Button bootButton(BOOT_FLAG_PIN);

enum KEY_VALUES { // Key codes for each button on the IR remote
    KEY_VOL_MINUS = 0,
    KEY_PLAY = 1,
    KEY_VOL_PLUS = 2,
    KEY_SETUP = 4,
    KEY_UP = 5,
    KEY_STOP = 6,
    KEY_LEFT = 8,
    KEY_ENTER = 9,
    KEY_RIGHT = 10,
    KEY_ZERO = 12,
    KEY_DOWN = 13,
    KEY_RETURN = 14,
    KEY_ONE = 16,
    KEY_TWO = 17,
    KEY_THREE = 18,
    KEY_FOUR = 20,
    KEY_FIVE = 21,
    KEY_SIX = 22,
    KEY_SEVEN = 24,
    KEY_EIGHT = 25,
    KEY_NINE = 26
};

enum ROBOT_STATE {
    ROBOT_IDLE,
    ROBOT_ACTIVE
};

// Declare a variable, robotState, of our new type, ROBOT_STATE. Initialize it to ROBOT_IDLE.
ROBOT_STATE robotState = ROBOT_IDLE;

void init();
void stop();

#endif
#include <Chassis.h>

// Class for the Chassis and everything on it (IR Detector, Drivetrain, Ultrasonic)

// Initializes Chassis fields
void init() {
    ultrasonic.attach(SIDE_ULTRASONIC_TRIG, SIDE_ULTRASONIC_ECHO);

    pinMode(LEFT_LINE_SENSE, INPUT);
    pinMode(RIGHT_LINE_SENSE, INPUT);

    // Initialize the button object
    bootButton.Init();

    // Initialize the decoder
    decoder.init();

    left_motor.attach();
    right_motor.attach();

    //Some motor admin; be sure to include this with any motor code
    //Pin definitions can be found in RBE1001Lib.h

    Serial.println("started");
}

// Emergency Stop
void stop() {
    Serial.println("EMERGENCY STOP -> ACTIVE");
    left_motor.setSpeed(0);
    right_motor.setSpeed(0);

    robotState = ROBOT_ACTIVE;
}
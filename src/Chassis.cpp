#include <Chassis.h>

// Class for the Chassis and everything on it (IR Detector, Drivetrain, Ultrasonic)
const uint8_t IR_DETECTOR_PIN = 15; // define the pin for the IR receiver
const uint8_t BOOT_BTN_PIN = 0;

float robotSpeed = 180; // Speed of the robot in ticks/s

IRDecoder decoder(IR_DETECTOR_PIN); // create an IRDecoder object
Button bootButton(BOOT_BTN_PIN); // Create a button object for the built-in button on the ESP32
Rangefinder ultrasonic;
Line line;

// Initializes Chassis fields
void Chassis::setup() {
    ultrasonic.attach(SIDE_ULTRASONIC_TRIG, SIDE_ULTRASONIC_ECHO);

    pinMode(LEFT_LINE_SENSE, INPUT);
    pinMode(RIGHT_LINE_SENSE, INPUT);

    // Initialize the button object
    bootButton.Init();

    // Initialize the decoder
    decoder.init();

    line.setup();

    Serial.println("started");
}

void Chassis::moveTo(float distInCm) {
    left_motor.setSpeed(robotSpeed);
    right_motor.setSpeed(robotSpeed);
}

void Chassis::moveFor(float speedInCmPerS) {
    float speed = 360 * sin(2 * 3.14 * (millis() % 10000) / 10000.);
	left_motor.setSpeed(speed);
	right_motor.setSpeed(speed);

	Serial.print("SP: ");
	Serial.print(speed);
	Serial.print('\t');

	Serial.print("L: ");
	Serial.print(left_motor.getDegreesPerSecond());
	Serial.print('\t');

	Serial.print("E: ");
	Serial.print(left_motor.getEffortPercent());
	Serial.print('\n');

	delay(100);
}

void Chassis::followPath(bool turnRightAtIntersection) {
    line.followLine(robotSpeed, &left_motor, &right_motor);
    // left_motor.setSpeed(robotSpeed);
    // right_motor.setSpeed(robotSpeed);
    // Serial.printf("millis time 2: %ld\n", millis());
}

void Chassis::turnToLine(bool turnRight) {
    
}

// Emergency Stop
void Chassis::stop() {
    Serial.println("EMERGENCY STOP");

    left_motor.setSpeed(0);
	right_motor.setSpeed(0);
}
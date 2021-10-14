#include <Chassis.h>

// Class for the Chassis and everything on it (Drivetrain)
const uint8_t BOOT_BTN_PIN = 0;

float robotSpeed = 180; // Speed of the robot in ticks/s

Button bootButton(BOOT_BTN_PIN); // Create a button object for the built-in button on the ESP32
Line line;

// Initializes Chassis fields
void Chassis::setup() {
    pinMode(LEFT_LINE_SENSE, INPUT);
    pinMode(RIGHT_LINE_SENSE, INPUT);

    bootButton.Init();

    line.setup();

    Serial.println("started");
}

bool Chassis::moveTo(float distInCm) {
    left_motor.setSpeed(robotSpeed);
    right_motor.setSpeed(robotSpeed);

    return true;
}

// bool Chassis::moveFor(float speedInCmPerS) {
// 	// Serial.print("SP: ");
// 	// Serial.print(speed);
// 	// Serial.print('\t');

// 	// Serial.print("L: ");
// 	// Serial.print(left_motor.getDegreesPerSecond());
// 	// Serial.print('\t');

// 	// Serial.print("E: ");
// 	// Serial.print(left_motor.getEffortPercent());
// 	// Serial.print('\n');

//     return true;
// }

int pathState = 0;

void Chassis::followPath(bool turnRightAtIntersection) {
    
    switch(pathState) {
        case 0:
            line.followLine(robotSpeed / 10, &left_motor, &right_motor);

            // if (line.checkForIntersection())
            //     pathState = 1;

            break;
        
        // case 1:
        //     if (turnToLine(turnRightAtIntersection))
        //         pathState = 0;

        //     break;
    }
}

bool Chassis::turnToLine(bool turnRight) {
    left_motor.setSpeed(turnRight ? robotSpeed/5 : -robotSpeed/5);
    right_motor.setSpeed(turnRight ? -robotSpeed/5 : robotSpeed/5);

    return line.checkNewLine(turnRight);
}

// Emergency Stop
void Chassis::stop() {
    Serial.println("EMERGENCY STOP");

    left_motor.setSpeed(0);
    right_motor.setSpeed(0);
}


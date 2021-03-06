#include <Chassis.h>

// Class for the Chassis and everything on it (Drivetrain)


// Initializes Chassis fields
void Chassis::setup() {
    pinMode(LEFT_LINE_SENSE, INPUT);
    pinMode(RIGHT_LINE_SENSE, INPUT);

    line.setup();

    Serial.println("started");
}

bool Chassis::moveTo(float distInCm) {
    // left_motor.setSpeed(robotSpeed);
    // right_motor.setSpeed(robotSpeed);

    return true;
}

void Chassis::setSpeed(float speed) {
    left_motor.setSpeed(speed);
    right_motor.setSpeed(speed);
}

void Chassis::turn(float speed, bool right) {
    left_motor.setSpeed(right ? speed : -speed);
    right_motor.setSpeed(right ? -speed : speed);
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

void Chassis::followPath(bool turnRightAtIntersection) {
    switch(pathState) {
        case 0:
            line.followLine(robotSpeed, &left_motor, &right_motor);

            if(line.checkForIntersection()) {
                stop();
                pathState = 1;
                startTime = millis();
            }

            break;
        
        case 1:
            if (millis() - startTime >= MAX_TURN_TIME) {
                pathState = 0;
                stop();
            } else {
                left_motor.setSpeed(turnRightAtIntersection ? robotSpeed * 1.4 : -robotSpeed * 1.4);
                right_motor.setSpeed(turnRightAtIntersection ? -robotSpeed * 1.4 : robotSpeed * 1.4);
            }

            break;
    }
}

bool Chassis::turnToLine(bool turnRight) {        //find the line on the other side of the field
    left_motor.setSpeed(turnRight ? robotSpeed : -robotSpeed);
    right_motor.setSpeed(turnRight ? -robotSpeed : robotSpeed);

    return line.checkNewLine(turnRight);
}

// Emergency Stop
void Chassis::stop() {
    left_motor.setSpeed(0);
    right_motor.setSpeed(0);
}


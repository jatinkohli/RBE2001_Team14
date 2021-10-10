// #include <Chassis.h>

// // Class for the Chassis and everything on it (IR Detector, Drivetrain, Ultrasonic)

// // Initializes Chassis fields
// void Chassis::setup() {
//     ultrasonic.attach(SIDE_ULTRASONIC_TRIG, SIDE_ULTRASONIC_ECHO);

//     pinMode(LEFT_LINE_SENSE, INPUT);
//     pinMode(RIGHT_LINE_SENSE, INPUT);

//     // Initialize the button object
//     bootButton.Init();

//     // Initialize the decoder
//     decoder.init();

//     left_motor.attach();
//     right_motor.attach();

//     line.setup();

//     Serial.println("started");
// }

// void Chassis::moveTo(float distInCm) {

// }

// void Chassis::followPath(bool turnRightAtIntersection) {

// }

// void Chassis::turnToLine(bool turnRight) {
    
// }

// // Emergency Stop
// void Chassis::stop() {
//     Serial.println("EMERGENCY STOP");

//     left_motor.setSpeed(0);
//     right_motor.setSpeed(0);
// }


#include <RBE1001Lib.h>
#include <IRdecoder.h>
#include <ESP32Servo.h>
#include <ESP32AnalogRead.h>

#include "BlueMotor.h"
#include "Chassis.h"
#include "Rangefinder.h"

const uint8_t IR_DETECTOR_PIN = 15; // define the pin for the IR receiver

//global variables and constants
BlueMotor blueMotor;
Chassis chassis;
Rangefinder ultrasonic;
IRDecoder decoder(IR_DETECTOR_PIN); // create an IRDecoder object
Servo gripper;
ESP32AnalogRead gripperFeedback;


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
    ArriveRoof,     //arrive at a roof 
    FirstVisit,     //25 degree position of the arm pickup and deposit
    SecondVisit,    //platform state for deposit and pickup
    ReturnRoof,     //return to the roof with the new panel
    LeaveRoof,      //leave for the platform deposit
    ReplaceCollector,
    SwitchSides
};

ROBOT_STATE robotState = ArriveRoof;    //initialize the state machine 

//--------------------------------------------------------------------------------------------------------------------------------
const int STAGING_POS = 620; //636;              //constants for positions of the arm
const int ROOF_PICKUP = 6122;
const int ROOF_25_PLACE = 7840; //7946;
const int ROOF_45_PLACE = 3550; //3600;

// cm for the ultrasonic to be from the object
const float OFFSET = 0.68; // offset to account for stopping time

const float STAGING_DIST = 5.6 + OFFSET;
const float ROOF_45_PLACE_DIST = 10.3 + OFFSET;
const float ROOF_45_PICKUP_DIST = 11.8 + OFFSET;
const float ROOF_25_PLACE_DIST = 7.0 + OFFSET;
const float ROOF_25_PICKUP_DIST = 5.5 + OFFSET;

const int GRIPPER_OPEN = 80;       // deg for servo
const int GRIPPER_CLOSED = 180;    // deg for servo

int effort = 0;
int setpoint = 0;

bool manualMode = false;

bool lineFollowing = false;
bool wasLineFollowing = false;

void setup() {
    Serial.begin(115200);

    chassis.setup();

    blueMotor.setup();
    blueMotor.reset();
    // ultrasonic.attach(21, 14); //attach the ultrasonic
    ultrasonic.attach(21, 3); //attach the ultrasonic
    // ultrasonic.attach(SIDE_ULTRASONIC_TRIG, SIDE_ULTRASONIC_ECHO); //attach the ultrasonic
    gripper.attach(SERVO_PIN);                                      //attach the gripper
    gripperFeedback.attach(SERVO_FEEDBACK_SENSOR);                  //attack the gripper feedback sensor

    decoder.init();

    effort = 0;
    setpoint = 0;

    manualMode = false;
    lineFollowing = false;
    wasLineFollowing = false;
}

float distance = 0;
float oldDistance = 0;

bool receivedKeyPress = false;
int prevSetpoint = setpoint;
bool paused = false;

bool on45Side = false;

int closeEnoughCount = 0;
int startTime = 0;

int numConfirmations = 0;
bool hasNewPlate = false;

bool readyToSwitch = false;

void loop() {
    // distance = ultrasonic.getDistanceCM(); //get the distance from the ultrasonic

    int key = decoder.getKeyCode();
    
    switch (robotState)        //state of the robot state machine in main()
    {
        case ArriveRoof: //arrive following the line at a roof

            if (receivedKeyPress) {
                blueMotor.setPosition((on45Side ? ROOF_45_PLACE : ROOF_25_PLACE));

                chassis.followPath(on45Side); //follow the line otherwise, if there is an intersection, turn left
                gripper.write(GRIPPER_OPEN);

                float dist = ultrasonic.getDistanceCM();
                Serial.println(dist);

                if (dist <= (on45Side ? ROOF_45_PICKUP_DIST : ROOF_25_PICKUP_DIST))  //45 grabbing position a little short
                    closeEnoughCount++;
                else
                    closeEnoughCount = 0;

                if (closeEnoughCount >= 5) {
                    robotState = FirstVisit; //set to the first position at a certain distance
                    receivedKeyPress = false;
                    closeEnoughCount = 0;

                    Serial.printf("Now in FirstVisit\n");
                    chassis.stop();
                }
            } else {
                receivedKeyPress = (key == KEY_LEFT || key == KEY_RIGHT);
                on45Side = key == KEY_RIGHT;
            }
            
            break;
        
        case FirstVisit:
            if (receivedKeyPress) {
                if (millis() - startTime >= 4100) {           //time might be too short because of battery, 45 eats itself
                    chassis.stop();                           //may or may not stop in 45, even with PLA
                    
                    receivedKeyPress = false;

                    startTime = millis();
                    robotState = LeaveRoof;
                } else if (millis() - startTime >= 3000) {
                    chassis.setSpeed(-chassis.robotSpeed);
                }

                blueMotor.setPosition(ROOF_PICKUP);
                Serial.println(blueMotor.getPosition());
            }

            if (key == KEY_ENTER) {                //this will always happen first because it has an important variable in it
                gripper.write(GRIPPER_CLOSED);

                startTime = millis();              //start the time count for driving backwards
                receivedKeyPress = true;           //set key press to false so that the ca\hassis can stop and leaeve the roof
            }

            break;

        case LeaveRoof:
            if (millis() - startTime >= 4600) {
                chassis.followPath(!on45Side);
                float dist = ultrasonic.getDistanceCM();

                if (dist <= STAGING_DIST)
                    closeEnoughCount++;
                else
                    closeEnoughCount = 0;

                if (closeEnoughCount >= 5) {
                    robotState = ReplaceCollector; //set to the first position at a certain distance
                    closeEnoughCount = 0;
                    numConfirmations = 0;
                    hasNewPlate = false;

                    chassis.stop();
                    Serial.printf("Now in ReplaceCollector\n");

                    startTime = millis();
                }
            } else {
                chassis.turn(chassis.robotSpeed, true);
            }

            break;

        case ReplaceCollector:
            if (millis() - startTime < 3000) {
                blueMotor.setPosition(STAGING_POS);
            } else if (key == KEY_ENTER) {
                numConfirmations++;

                if (numConfirmations == 1) {
                    gripper.write(GRIPPER_OPEN);
                } else if (numConfirmations == 2) {
                    gripper.write(GRIPPER_CLOSED);
                    
                    hasNewPlate = true;
                    startTime = millis();
                }
            } else if (hasNewPlate) {
                if (millis() - startTime >= 13200) {
                    chassis.stop();
                    hasNewPlate = false;
                    robotState = ReturnRoof;
                } else if (millis() - startTime >= 10000) {
                    blueMotor.setPosition(ROOF_PICKUP);
                    chassis.turn(chassis.robotSpeed * 1.5, false);
                } else {
                    blueMotor.setPosition(ROOF_PICKUP);
                    chassis.stop();
                }
            }

            break;

        case SecondVisit:                   //starts the state where the panel is dropped off at the h
            if (millis() - startTime >= 3000) {
                if (receivedKeyPress) {
                    if (millis() - startTime >= 4000) {          //time running until the end of the state
                        chassis.stop();
                        
                        receivedKeyPress = false;
                        readyToSwitch = false;
                        closeEnoughCount = 0;

                        startTime = millis();
                        robotState = SwitchSides;
                    }

                    chassis.setSpeed(-chassis.robotSpeed);
                }

                if (key == KEY_ENTER) {                //this will always happen first because it has an important variable in it
                    gripper.write(GRIPPER_OPEN);

                    startTime = millis();              //start the time count for driving backwards
                    receivedKeyPress = true;           //set key press to false so that the ca\hassis can stop and leaeve the roof
                }
            } else {
                blueMotor.setPosition((on45Side ? ROOF_45_PLACE : ROOF_25_PLACE));
            }

            break;

        case ReturnRoof: {                                 //return to the roof for the deposit of the new panel
            chassis.followPath(on45Side); //follow the line otherwise, if there is an intersection, turn left
            gripper.write(GRIPPER_CLOSED);

            float dist = ultrasonic.getDistanceCM();
            Serial.println(dist);

            if (dist <= (on45Side ? ROOF_45_PLACE_DIST : ROOF_25_PLACE_DIST)) {
                closeEnoughCount++;
            }
            else {
                closeEnoughCount = 0;
            }

            if (closeEnoughCount >= 5) {
                robotState = SecondVisit; //set to the first position at a certain distance
                closeEnoughCount = 0;

                startTime = millis();
                receivedKeyPress = false;

                Serial.printf("Now in SecondVisit\n");
                chassis.stop();
            }

        }
        break;

        case SwitchSides: {

            int mod = on45Side ? 1 : -1;

            chassis.left_motor.startMoveFor(mod * 180, 100);
            chassis.right_motor.moveFor(mod * -180, 100);

            long startTime = millis();
            while ((millis() - startTime) < 5000)
                chassis.followPath(!on45Side);

            chassis.left_motor.startMoveFor(mod * -180, 100);
            chassis.right_motor.moveFor(mod * 180, 100);

            startTime = millis();
            while ((millis() - startTime) < 13000)
                chassis.followPath(!on45Side);

            chassis.left_motor.startMoveFor(mod * -45, 100);
            chassis.right_motor.moveFor(mod * 45, 100);

            on45Side = !on45Side;

            chassis.stop();

            robotState = ArriveRoof;

            break;
        }   
    }

    if (key == KEY_TWO) {     //estop
        paused = !paused;

        Serial.println("EMERGENCY STOP");

        if (paused) {
            prevSetpoint = setpoint;
            setpoint = blueMotor.getPosition();
            chassis.stop();

            wasLineFollowing = lineFollowing;
            lineFollowing = false;
        } else {
            setpoint = prevSetpoint;
            prevSetpoint = setpoint;

            lineFollowing = wasLineFollowing;
        }
    } 
    // else if (key == KEY_VOL_PLUS) { //servo?
    //     effort++;
    // } else if (key == KEY_STOP) { //
    //     effort += 10;
    // }
    // else if (key == KEY_RIGHT) {       //blue motor arm down
    //     effort += 100;
    // }
    // else if (key == KEY_VOL_MINUS) {
    //     effort--;
    // }
    // else if (key == KEY_RETURN) {
    //     lineFollowing = true;
    // }else if (key == KEY_ZERO) {
    //     lineFollowing = false;
    // }
    // else if (key == KEY_SETUP) {
    //     effort -= 10;
    // }
    // else if (key == KEY_LEFT) {         //blue motor arm up
    //     effort -= 100;
    // }
    // else if (key == KEY_PLAY) {         //preset staging platform button and position
    //     setpoint = STAGING_POS;
    // }
    // else if (key == KEY_UP) {           //preset 25 degree roof button and position for pickup
    //     setpoint = ROOF_25_PICKUP;
    // }
    // else if (key == KEY_ENTER) {        //preset 25 degreee roof button and position for placement
    //     setpoint = ROOF_25_PLACE;
    // }
    // else if (key == KEY_DOWN) {         //preset staging platform button and position
    //     setpoint = ROOF_45_PLACE;
    // }
    // else if (key == KEY_SEVEN) {        //perset servo gripper open
    //     gripper.write(GRIPPER_OPEN);
    // }
    // else if (key == KEY_NINE) {         //preset gripper closed
    //     gripper.write(GRIPPER_CLOSED);
    // } else if (key == KEY_FIVE) {              //take the robot out of manual mode
    //     manualMode = !manualMode;

    delay(10); // Run everything (state machine, PID loops, etc.) at 100 Hz
}



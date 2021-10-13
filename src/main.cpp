#include <RBE1001Lib.h>
#include <IRdecoder.h>
#include <ESP32Servo.h>
#include <ESP32AnalogRead.h>

#include "BlueMotor.h"
#include "Chassis.h"

//code for RBE 2001 lab4

const uint8_t IR_DETECTOR_PIN = 15; // define the pin for the IR receiver

//global variables and constants
BlueMotor blueMotor;
Chassis chassis;
IRDecoder decoder(IR_DETECTOR_PIN); // create an IRDecoder object
Servo gripper;
ESP32AnalogRead gripperFeedback;

enum KEY_VALUES
{ // Key codes for each button on the IR remote
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

enum ROBOT_STATE
{
    //ROBOT_IDLE,   //robot is waiting for the button press to start it
    //ROBOT_ACTIVE, //press a button and the robot activates
    firstp,       //45 degree position of the arm
    secondp,      //25 degee position of the arm
    UpLow,         //deposit on the 2 inch platform
    LowUp,        //retrieve the solar panel from the low platform
    Arriveroof,   //arrive at a roof to pickup a solar pannel
    ReturnRoof,   //return to the roof in order to deposit the solar panel
    LeaveRoof,    //leave the 45 degree roof in order to deposit the solar panel
    roofleave,    //leave the 25 degree roof to deposit the solar pannel
    
};

// Declare a variable, robotState, of our new type, ROBOT_STATE. Initialize it to ROBOT_IDLE.
ROBOT_STATE robotState = firstp; //should be ROBOT_IDLE, but is changed for lab 4

const int STAGING_POS = 0;                   //constants for positions of the arm
const int ROOF_25_PICKUP = -7078;      //+226 because an adjustment had ot be made from the origional measurement
const int ROOF_25_PLACE = -6346;

const int GRIPPER_OPEN = 80; // deg for servo
const int GRIPPER_CLOSED = 180; // deg for servo

void setup()
{
    Serial.begin(9600);

    chassis.setup();

    blueMotor.setup();
    blueMotor.reset();

    gripper.attach(SERVO_PIN);
    gripperFeedback.attach(SERVO_FEEDBACK_SENSOR);

    decoder.init();
}

int effort = 0;
int setpoint = 0;

void loop() { 
    int key = decoder.getKeyCode();

    if (key == KEY_VOL_PLUS) {
        effort++;
    } else if (key == KEY_STOP) {
        effort += 10;
    } else if (key == KEY_RIGHT) {
        effort += 100;
    } else if (key == KEY_VOL_MINUS) {
        effort--;
    } else if (key == KEY_SETUP) {
        effort -= 10;
    } else if (key == KEY_LEFT) {
        effort -= 100;
    } else if (key == KEY_PLAY) {                         //staging platform button and position
        setpoint = STAGING_POS;
    } else if (key == KEY_UP) {                           //25 degree roof button and position for pickup
        setpoint = ROOF_25_PICKUP;
    } else if (key == KEY_ENTER) {                        //25 degreee roof button and position for placement
        setpoint = ROOF_25_PLACE;
    } else if (key == KEY_SEVEN) {
        gripper.write(GRIPPER_OPEN);
    } else if (key == KEY_NINE) {
        gripper.write(GRIPPER_CLOSED);
    }

    effort = constrain(effort, -255, 255);

    blueMotor.setEffortCorrected(effort);
    blueMotor.setPosition(setpoint);
    
    float slope = (255.0 - 77.0) / 255.0;
    int yInt = 77;
    int correctedEffort;

    if (effort < 0) {
        correctedEffort = (int)(slope * effort) - yInt;
    } else if (effort > 0) {
        correctedEffort = (int)(slope * effort) + yInt;
    } else {
        correctedEffort = 0;
    }

    float rpm = 169.98 * correctedEffort / 255.0;
    rpm = constrain(rpm, -169.98, 169.98);

    Serial.printf("%ld | %d | %d | %f\n", millis(), effort, correctedEffort, rpm);
    // Serial.printf("%d | %ld\n", effort, blueMotor.getPosition());   

    delay(10);

    // int key = decoder.getKeyCode();

    // BlueMotor.setEffort(90);        //2 inch bench

    // switch (robotState)
    // {
    // case firstp:                   //sets the state to the 45 degree angle
    //     blueMotor.setPosition(90); //use PID in encoders to put into position specified
    //     blueMotor.stopMotor();
    //     break; //break out of the state machine

    // case secondp:                  //sets the state to the 25 degree angle
    //     blueMotor.setPosition(70); //use PID in encoders to put into position specified
    //     break;                     //break out of the state machine

    // case UpLow:                      //sets the state to the platform
    //     blueMotor.setPosition(110);  //use PID in encoders to put into position specified
    //     break;                       //break out of the state machine

       //case ArriveRoof:                 //arrive following the line at a roof
           //follow a line 
           //if a certain distance, stop and adjust to corrrect distance
           //switch state to fisrtp,
    

    // }

    // float time = millis();

    // if (time - prevSetTime >= 100)
    // {
    //     blueMotor.setEffort(++effort);
    //     delay(100);
    //     prevSetTime = time;
    // }

    // Serial.printf("%ld | %d\n", blueMotor.getPosition(), effort);
}
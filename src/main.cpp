#include <RBE1001Lib.h>
#include <IRdecoder.h>

#include "BlueMotor.h"
#include "Chassis.h"

//code for RBE 2001 lab4

const uint8_t IR_DETECTOR_PIN = 15; // define the pin for the IR receiver

BlueMotor blueMotor;
Chassis chassis;
IRDecoder decoder(IR_DETECTOR_PIN); // create an IRDecoder object

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
    ROBOT_IDLE,   //robot is waiting for the button press to start it
    ROBOT_ACTIVE, //press a button and the robot activates
    firstp,       //45 degree position of the arm
    secondp,      //25 degee position of the arm
    lowp,         //deposit on the 2 inch platform
    lab
};

// Declare a variable, robotState, of our new type, ROBOT_STATE. Initialize it to ROBOT_IDLE.
ROBOT_STATE robotState = firstp; //should be ROBOT_IDLE, but is changed for lab 4

int effort = 0;

void setup()
{
    Serial.begin(9600);

    chassis.setup();

    blueMotor.setup();
    blueMotor.reset();

    decoder.init();
}

void loop()
{ //remember to stop effort before 255
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
    }

    effort = constrain(effort, -255, 255);

    blueMotor.setEffortCorrected(effort);

    float slope = (255.0 - 77.0) / 255.0;
    int yInt = 77;
    int correctedEffort;

    if (effort < 0) {
        correctedEffort = -((int)(slope * -effort) + yInt);
    } else {
        correctedEffort = (int)(slope * effort) + yInt;
    }

    float rpm = 169.98 * effort / 255.0;
    rpm = constrain(rpm, -169.98, 169.98);

    Serial.printf("%ld | %d | %d | %f\n", millis(), effort, correctedEffort, rpm);

    delay(5);

    // switch (robotState)
    // {
    // case firstp:                   //sets the state to the 45 degree angle
    //     blueMotor.setPosition(90); //use PID in encoders to put into position specified
    //     blueMotor.stopMotor();
    //     break; //break out of the state machine

    // case secondp:                  //sets the state to the 25 degree angle
    //     blueMotor.setPosition(70); //use PID in encoders to put into position specified
    //     break;                     //break out of the state machine

    // case lowp:                      //sets the state to the platform
    //     blueMotor.setPosition(110); //use PID in encoders to put into position specified
    //     break;                      //break out of the state machine

    // case lab:
    //     blueMotor.incrementEffort(0);
    //     break;
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
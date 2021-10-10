#include <RBE1001Lib.h>

#include "BlueMotor.h"

//code for RBE 2001 lab4

BlueMotor blueMotor;

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

float prevSetTime = 0;
int effort = 0;

void setup()
{
    Serial.begin(115200);
    blueMotor.setup();
    blueMotor.reset();
}

void loop()
{ //remember to stop effort before 255

    switch (robotState)
    {
    case firstp:                   //sets the state to the 45 degree angle
        blueMotor.setPosition(90); //use PID in encoders to put into position specified
        blueMotor.stopMotor();
        break; //break out of the state machine

    case secondp:                  //sets the state to the 25 degree angle
        blueMotor.setPosition(70); //use PID in encoders to put into position specified
        break;                     //break out of the state machine

    case lowp:                      //sets the state to the platform
        blueMotor.setPosition(110); //use PID in encoders to put into position specified
        break;                      //break out of the state machine

    case lab:
        blueMotor.incrementEffort(0);
        break;
    }

    float time = millis();

    if (time - prevSetTime >= 100)
    {
        blueMotor.setEffort(++effort);
        delay(100);
        prevSetTime = time;
    }

    //Serial.printf("%ld | %d\n", blueMotor.getPosition(), effort);
}
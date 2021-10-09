#include <RBE1001Lib.h>
#include <IRdecoder.h>

#include "BlueMotor.h"
#include "Chassis.h"

//code for RBE 2001 lab4

const uint8_t IR_DETECTOR_PIN = 15; // define the pin for the IR receiver

BlueMotor blueMotor;
Chassis chassis;
IRDecoder decoder(IR_DETECTOR_PIN); // create an IRDecoder object

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

int effort = 0;

void setup() {
    Serial.begin(115200);

    chassis.setup();

    blueMotor.setup();
    blueMotor.reset();

    decoder.init();
}

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
    }

    Serial.printf("%d | %ld\n", effort, blueMotor.getPosition());
    blueMotor.setEffort(effort);

    delay(5);
}
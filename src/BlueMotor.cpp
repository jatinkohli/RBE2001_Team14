#include "BlueMotor.h"
#include <RBE1001Lib.h>

long count = 0; // encoder counter
// Mutex for the count critical variable
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

/**
 * Interrupt service routine (ISR) for one of the encoder
 * channels. It simply counts how many interrupts occured
 */
void IRAM_ATTR isr() {
    portENTER_CRITICAL_ISR(&mux);
    count++;
    portEXIT_CRITICAL_ISR(&mux);
}

/**
 * Set up all the variables for the blue motor
 * This is not the same as the ESP32 setup() function, although
 * that would be a good place to call this setup function
 */
void BlueMotor::setup() {
    pinMode(ENCA, INPUT);
    pinMode(ENCB, INPUT);
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    pinMode(PWM, OUTPUT);
    attachInterrupt(ENCA, isr, CHANGE);
}

/**
 * Get the current encoder count
 * Returns the encoder count value
 */
long BlueMotor::getPosition() {
    long value;
    portENTER_CRITICAL(&mux);
    value = count;
    portEXIT_CRITICAL(&mux);
    return value;
}

/**
 * Reset the encoder count to zero
 */
void BlueMotor::reset() {
    portENTER_CRITICAL(&mux);
    count = 0;
    portEXIT_CRITICAL(&mux);
}

/**
 * Set the motor effort
 * effort value ranges from -255 to +255
 * Negative values turn one way, positive values turn the other way
 */
void BlueMotor::setEffort(int effort) {
    if (effort < 0) {
        setEffort(-effort, true);
    } else {
        setEffort(effort, false);
    }
}

/**
 * Set the motor effort
 * effort values range from 0-255
 * clockwise is true for one direction, false for the other
 */
void BlueMotor::setEffort(int effort, bool clockwise) {
    if (clockwise) {
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
    } else {
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
    }
    int value = constrain(effort, 0, 255);
    analogWrite(PWM, value);
}


// int incrementEffort(int effort) {
//     for(int i = 0; i <= 255; i++) {
//         setEffort(i);
//     }
//     return effort;
// }


//moveFor is blocking?  moves to a cerain number of degrees
//moveTo is not a blocking function? moves to a position
//to spin both wheels at the same time ,one moveTo and then one moveFor
//remember to account for the wheel diameter and the circle diameter in the turns of the robot
//there is documentation of the lab code in canbavas undder a lecture somewhere
//you can set a limit to the integral term 

//use the aPID class from the library for the blue motor stuff, Brad Miller said in lab
//PID_V1 search in libraries tab in platform.io
//use the one from brett buaeregard
//then click "add to project"

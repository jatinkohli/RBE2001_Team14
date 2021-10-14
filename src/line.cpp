#include "line.h"
#include <ESP32AnalogRead.h>

unsigned long lastTime = 0;

void Line::setup() {
    pinMode(LEFT_LINE_SENSE, INPUT);                //pin sensor
    pinMode(RIGHT_LINE_SENSE, INPUT);               //pin sensor
}

void Line::followLine(float baseSpeed, LeftMotor* left_motor, RightMotor* right_motor) {                           //line following      

    //unsigned long currTime = millis();

    Serial.printf("%d | %d\n", analogRead(LEFT_LINE_SENSE), analogRead(RIGHT_LINE_SENSE));      //print the values

    //if(currTime - lastTime > LINE_FOLLOWING_INTERVAL) {
        //read sensors
        int leftReading = analogRead(LEFT_LINE_SENSE);           //analog reading for the pin
        int rightReading = analogRead(RIGHT_LINE_SENSE);         //analog reading for the pin

        //calculate error
        int error = rightReading - leftReading;             //innitial error reading
        static int errorAcc = rightReading - leftReading;   //for the ki value

        //calculate "effort"
        float effort = error * kp;    //kp currently 0.004

        //command the motors
        left_motor->setSpeed(baseSpeed + effort + errorAcc*-ki);   //calculation for wheel speed and wheel turn
        right_motor->setSpeed(baseSpeed + effort + errorAcc*ki);    //calculation for wheel speed and wheel turn

        errorAcc = error + errorAcc;            //updating the accumulated error for the ki value

        //lastTime = currTime;
    //}
}



bool Line::checkForIntersection() {                 //check for intersection
    
    static int leftPrev = 0;
    static int rightPrev = 0;

    unsigned long currTime = millis(); 

    if(currTime - lastTime > LINE_FOLLOWING_INTERVAL) {
        //read sensors
        int leftReading = analogRead(LEFT_LINE_SENSE);      //pin attach
        int rightReading = analogRead(RIGHT_LINE_SENSE);    //pin attach

        //int error = leftReading- rightReading;              //error for turning on an intersection
            
        //Check left sensor
        if(leftReading >= threshold && leftPrev > threshold) {
            //If left sensor meets threshold, watch for event when right meets threshold
            return rightReading >= threshold && rightPrev < threshold;
        } else if(rightReading > threshold) { //Check right sensor
            //If right sensor meets threshold, watch for event when left meets threshold
            return leftPrev < threshold && leftReading >= threshold; 
        }

        rightPrev = rightReading;       //previous reading from each line sensor
        leftPrev = leftReading;         //previous reading from each line sensor
    }

    return false;
}
//;)
//Checks for when romi has found a new line after turning at an intersection
bool Line::checkNewLine(bool right) {                                                                //find the next line
    static int leftPrev2 = 0;
    static int rightPrev2 = 0;
    
    unsigned long currTime = millis(); 
    if(currTime - lastTime > LINE_FOLLOWING_INTERVAL) {
        //read sensors
        int leftReading = analogRead(LEFT_LINE_SENSE);
        int rightReading = analogRead(RIGHT_LINE_SENSE);
        
        if(right) {
            if((leftReading < threshold && rightReading > threshold) && (leftPrev2 >= threshold || rightPrev2 <= threshold)) {
                return true;
            }
        } else {
            if((leftReading > threshold && rightReading < threshold) && (leftPrev2 <= threshold || rightPrev2 >= threshold)) {
                return true;
            }
        }

        rightPrev2 = rightReading;
        leftPrev2 = leftReading;
    }
    
    return false;
}
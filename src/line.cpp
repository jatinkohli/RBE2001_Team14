#include "line.h"

void Line::setup() {
    pinMode(LEFT_LINE_SENSE, INPUT);
    pinMode(RIGHT_LINE_SENSE, INPUT);
}

void Line::followLine(float baseSpeed, Motor left_motor, Motor right_motor) {                                                    //line following
    unsigned long currTime = millis();

    if(currTime - lastTime > LINE_FOLLOWING_INTERVAL) {
        //read sensors
        int leftReading = analogRead(LEFT_LINE_SENSE);
        int rightReading = analogRead(RIGHT_LINE_SENSE);

        //calculate error
        int error = rightReading - leftReading;

        //calculate "effort"
        float effort = error * kp;

        //command the motors
        left_motor.setSpeed(baseSpeed - effort);
        right_motor.setSpeed(baseSpeed + effort);
    }
}

bool Line::checkForIntersection() {                                                                      //check for intersection
    static int leftPrev = 0;
    static int rightPrev = 0;

    unsigned long currTime = millis(); 
    if(currTime - lastTime > LINE_FOLLOWING_INTERVAL) {
        //read sensors
        int leftReading = analogRead(LEFT_LINE_SENSE);
        int rightReading = analogRead(RIGHT_LINE_SENSE);
            
        //Check left sensor
        if(leftReading >= threshold && leftPrev > threshold) {
            //If left sensor meets threshold, watch for event when right meets threshold
            return rightReading >= threshold && rightPrev < threshold;
        } else if(rightReading > threshold) { //Check right sensor
            //If right sensor meets threshold, watch for event when left meets threshold
            return leftPrev < threshold && leftReading >= threshold; 
        }

        rightPrev = rightReading;
        leftPrev = leftReading;
    }

    return false;
}

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
#include "line.h"

void followLine(float baseSpeed) {                                                    //line following
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

bool checkForIntersection() {                                                                      //check for intersection
    static int leftPrev = 0;
    static int rightPrev = 0;

    unsigned long currTime = millis(); 
    if(currTime - lastTime > LINE_FOLLOWING_INTERVAL) {
        //read sensors
        int leftReading = analogRead(LEFT_LINE_SENSE);
        int rightReading = analogRead(RIGHT_LINE_SENSE);
            
        //Check left sensor
        if(leftReading >= threshhold && leftPrev > threshhold) {
            //If left sensor meets threshold, watch for event when right meets threshold
            return rightReading >= threshhold && rightPrev < threshhold;
        } else if(rightReading > threshhold) { //Check right sensor
            //If right sensor meets threshold, watch for event when left meets threshold
            return leftPrev < threshhold && leftReading >= threshhold; 
        }

        rightPrev = rightReading;
        leftPrev = leftReading;
    }

    return false;
}

//Checks for when romi has found a new line after turning at an intersection
bool checkNewLine(bool right) {                                                                //find the next line
    static int leftPrev2 = 0;
    static int rightPrev2 = 0;
    
    unsigned long currTime = millis(); 
    if(currTime - lastTime > LINE_FOLLOWING_INTERVAL) {
        //read sensors
        int leftReading = analogRead(LEFT_LINE_SENSE);
        int rightReading = analogRead(RIGHT_LINE_SENSE);
        
        if(right) {
            if((leftReading < threshhold && rightReading > threshhold) && (leftPrev2 >= threshhold || rightPrev2 <= threshhold)) {
                return true;
            }
        } else {
            if((leftReading > threshhold && rightReading < threshhold) && (leftPrev2 <= threshhold || rightPrev2 >= threshhold)) {
                return true;
            }
        }

        rightPrev2 = rightReading;
        leftPrev2 = leftReading;
    }
    
    return false;
}
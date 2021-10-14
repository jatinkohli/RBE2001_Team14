#include "line.h"

unsigned long lastTime = 0;

void Line::setup() {
    pinMode(LEFT_LINE_SENSE, INPUT);                //pin sensor
    pinMode(RIGHT_LINE_SENSE, INPUT);               //pin sensor
}

void Line::followLine(float baseSpeed, LeftMotor* left_motor, RightMotor* right_motor) {  //line following                                            
    unsigned long currTime = millis();

    Serial.printf("%d | %d\n", analogRead(LEFT_LINE_SENSE), analogRead(RIGHT_LINE_SENSE));      //print the values

    if(currTime - lastTime > LINE_FOLLOWING_INTERVAL) {
        //read sensors
        int leftReading = analogRead(LEFT_LINE_SENSE);           //analog reading for the pin
        int rightReading = analogRead(RIGHT_LINE_SENSE);         //analog reading for the pin

        //calculate error
        int error = rightReading - leftReading;

        //calculate "effort"
        float effort = error * kp;    //kp currently 0.004

        //command the motors
        // left_motor->setSpeed(baseSpeed - effort);
        // right_motor->setSpeed(baseSpeed + effort);

        lastTime = currTime;
    }
}



// bool detectIntersection()                                // intersection detection function
// {
// bool  isIntersection = false;

// int adclinesensor2 = analogRead(linesensor1Pin);
// int adclinesensor1 = analogRead(linesensor2Pin);

// static int prevAdclinesensor2 = analogRead(linesensor1Pin);
// static int prevAdclinesensor1 = analogRead(linesensor2Pin);

// Serial.print(adclinesensor1);
// Serial.print('\t');
// Serial.print(adclinesensor2);
// Serial.print('\t');
// Serial.print(prevAdclinesensor1);
// Serial.print('\t');
// Serial.println(prevAdclinesensor2);

// int error = adclinesensor1-adclinesensor2;

// if(adclinesensor2 > 1200 && adclinesensor1 > 1200 && adclinesensor2 - prevAdclinesensor2 < 1000 && adclinesensor1 - prevAdclinesensor1 < 1000)   // this determines if the intersection is true based on 
// {                                                                                                                                                // previous and current line sensor readings
//   isIntersection=true;
// }

// prevAdclinesensor1 = adclinesensor1;                   // the previous readinf from each line sensor             
// prevAdclinesensor2 = adclinesensor2;

// return isIntersection;                                 // returns the boolean value of if the intersectio is an intersection

//====================================================================================================================================

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
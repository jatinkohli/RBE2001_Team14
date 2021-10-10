#include "BlueMotor.h"

// fields for quadrature decoding
const int X = 5;
int encoderArray[4][4] = {    
        {0, -1, 1, X},    
        {1, 0, X, -1},    
        {-1, X, 0, 1},    
        {X, 1, -1, 0}
};

int oldValue = 0, newValue = 0, errorCount = 0;
long count = 0; // encoder counter

/**
 * Interrupt service routine (ISR) for one of the encoder
 * channels. It simply counts how many interrupts occured
 */
void isr() {  
    newValue = (digitalRead(3) << 1) | digitalRead(2);
    int value = encoderArray[oldValue][newValue];
    if (value == X) {    
        errorCount++;  
    } else {    
        count -= value;  
    }  
    
    oldValue = newValue;
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
    attachInterrupt(ENCB, isr, CHANGE);
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
void BlueMotor::setEffort(int effort, bool clockwise) {       //this is a private version defined insode the class of BlueMotor
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


// positionIdeal has the same units as the encoder count
void BlueMotor::setPosition(int positionIdeal) {             //beginning of the PID control for the encoders/arm positions
                                                             //input the desired position to the function for the arm
    if (prevSetpoint != positionIdeal) { // when going to new position, reset accumulated error
        errorSum = 0;
        prevSetpoint = positionIdeal;
    }
    
    int effort = 0;                                  
   
    int error = positionIdeal - getPosition();

    effort += kp * error; // P
    effort += ki * errorSum; // I
    effort += kd * (error - errorThen); // D

    setEffort(effort);
    

    errorSum += error;
    errorThen = error;

    Serial.print(errorThen);
    Serial.print("\t");
    Serial.print(error);
    Serial.print("\t");
    Serial.print(getPosition());
    Serial.print("\t");
    Serial.println(effort);
}

void BlueMotor::stopMotor(){                              //stop the motor
    setEffort(0);
}


 int BlueMotor::incrementEffort(int effort) {
     for(int i = 0; i <= 255; i++) {
         setEffort(i);
         //Serial.println(effort,  count);
     }
     return effort;
 }


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

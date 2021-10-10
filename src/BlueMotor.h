#pragma once

#include <Arduino.h>
#include <RBE1001Lib.h>

class BlueMotor {
    public:
        void setEffort(int effort);
        long getPosition();
        void reset();
        void setup();
        void setPosition(int positionIdeal);
        void stopMotor();
        int incrementEffort(int effort);

    private:
        const int PWM = 5;
        const int AIN2 = 23;
        const int AIN1 = 35;    //changed from 27 because 27 was connected to the left drive motor
        const int ENCA = 19;
        const int ENCB = 18;

        // for position PID
        float kp = .3;
        float ki = 0; //1.0;
        float kd = 0; //0.009;
        int errorThen;            //error from the previous loop
        int errorSum;             //sum of the error?
        int prevSetpoint = 0;     
        
        void setEffort(int effort, bool clockwise);

        // Mutex for the count critical variable
        portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
};
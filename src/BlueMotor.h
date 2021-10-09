#pragma once

#include <Arduino.h>
#include <RBE1001Lib.h>

class BlueMotor {
    public:
        void setEffort(int effort);
        long getPosition();
        void reset();
        void setup();

    private:
        const int PWM = 5;
        const int AIN2 = 23;
        const int AIN1 = 27;
        const int ENCA = 19;
        const int ENCB = 18;
        
        void setEffort(int effort, bool clockwise);

        // Mutex for the count critical variable
        portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
};
#include <RBE1001Lib.h>

#include "BlueMotor.h"
#include "line.h"

//code for RBE 2001 lab4

BlueMotor blueMotor;

float prevSetTime = 0;
int effort = 0;
float count = 0;

void setup() {
    Serial.begin(115200);
    blueMotor.setup();
    blueMotor.reset();
}

void loop() {                          //remember to stop it before 255
    float time = millis();

    if (time - prevSetTime >= 100) {
        blueMotor.setEffort(++effort);
        prevSetTime = time;
    }

    count = blueMotor.getPosition();
    Serial.printf("%f | %d\n", count, effort); 
}
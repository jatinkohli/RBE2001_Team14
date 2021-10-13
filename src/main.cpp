#include <RBE1001Lib.h>
#include <IRdecoder.h>
#include <ESP32Servo.h>
#include <ESP32AnalogRead.h>

#include "BlueMotor.h"
#include "Chassis.h"
#include "Rangefinder.h"

const uint8_t IR_DETECTOR_PIN = 15; // define the pin for the IR receiver

//global variables and constants
BlueMotor blueMotor;
Chassis chassis;
Rangefinder ultrasonic;
IRDecoder decoder(IR_DETECTOR_PIN); // create an IRDecoder object
Servo gripper;
ESP32AnalogRead gripperFeedback;


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

    FirstVisit,     //25 degree position of the arm pickup and deposit
    SecondVisit,    //platform state for deposit and pickup
    ArriveRoof,     //arrive at a roof 
    ReturnRoof,     //return to the roof with the new panel
    LeaveRoof,      //leave for the platform deposit
    DistanceTest    //test the ultrasonic distance function
    

};

ROBOT_STATE robotState = DistanceTest;    //innitialize the state machine 

//--------------------------------------------------------------------------------------------------------------------------------
const int STAGING_POS = 0;              //constants for positions of the arm
const int ROOF_25_PICKUP = -7078;       //+226 because an adjustment had ot be made from the origional measurement
const int ROOF_25_PLACE = -6346;

const int GRIPPER_OPEN = 80;       // deg for servo
const int GRIPPER_CLOSED = 180;    // deg for servo

void setup() {
    Serial.begin(115200);

    chassis.setup();

    blueMotor.setup();
    blueMotor.reset();
    ultrasonic.attach(21, 14); //attach the untrasonic
    // ultrasonic.attach(SIDE_ULTRASONIC_TRIG, SIDE_ULTRASONIC_ECHO); //attach the untrasonic
    gripper.attach(SERVO_PIN);                                      //attach the gripper
    gripperFeedback.attach(SERVO_FEEDBACK_SENSOR);                  //attack the gripper feedback sensor

    decoder.init();
}

int effort = 0;
int setpoint = 0;
float distance = 0;

bool manualMode = false;

void loop()
{
    distance = ultrasonic.getDistanceCM(); //get the distance from the untrasoonic
    Serial.println(distance);

    int key = decoder.getKeyCode();
    //Serial.println(key);
    if (key == KEY_VOL_PLUS)
    { //servo?
        effort++;
    }
    else if (key == KEY_STOP){ //
        effort += 10;
    }
    else if (key == KEY_RIGHT) {       //blue motor arm down
        effort += 100;
    }
    else if (key == KEY_VOL_MINUS){
        effort--;
    }
    else if (key == KEY_SETUP){
        effort -= 10;
        //key == KEY_LEFT)
    }
    else if (key == KEY_LEFT){          //blue motor arm up
        effort -= 100;
        //Serial.println(effort);
    }
    else if (key == KEY_PLAY){         //preset staging platform button and position
        setpoint = STAGING_POS;
    }
    else if (key == KEY_UP){           //preset 25 degree roof button and position for pickup
        setpoint = ROOF_25_PICKUP;
    }
    else if (key == KEY_ENTER){         //preset 25 degreee roof button and position for placement
        setpoint = ROOF_25_PLACE;
    }
    else if (key == KEY_SEVEN){        //perset servo gripper open
        gripper.write(GRIPPER_OPEN);
    }
    else if (key == KEY_NINE){         //preset gripper closed
        gripper.write(GRIPPER_CLOSED);
    } else if (key == KEY_FIVE) {
        manualMode = !manualMode;
        
        Serial.printf("ManualMode: %d\n", manualMode);
    }

    effort = constrain(effort, -255, 255);

    //(not both at the same time)
    if (manualMode)
        blueMotor.setEffortCorrected(effort); //use the corrected effort in order to place the arm manually
    else
        blueMotor.setPosition(setpoint); //use the perdetermined locations to position the arm

    // Serial.printf("ultra dist: %.1f | %d | %ld\n", distance, effort, blueMotor.getPosition());

    delay(10);

    // int key = decoder.getKeyCode();
//---------------------------------------------------------------------------------------------------------------------------------
    switch (robotState)        //state of the robot state machine in main()
    {

        case ArriveRoof: //arrive following the line at a roof
            if (ultrasonic.getDistanceCM() <= 30){
                robotState = FirstVisit; //set to the first position at a certain distance
            }
            else{
                chassis.followPath(false); //follow the line otherwise, if there is an intersection, turn left
            }  
            break;
        
        case FirstVisit:
            gripper.write(GRIPPER_OPEN);            //set the gripper to open
            setpoint = ROOF_25_PLACE;

            delay(10);
            //adjust distance line  for best pickup drive foward
            gripper.write(GRIPPER_CLOSED);          //set the gripper to closed
            setpoint = ROOF_25_PICKUP;

            //set distance to backup a few

            robotState = LeaveRoof; //go to the next state in order to turn and leave the roof location

            break;


        case LeaveRoof:
            //turn            //(find until the new line is located, then next function takes over)
            //line.checkNewLine(right);            //determine if the robot is on a new line
            //follow line
            //turn (right?) at intersection
            //follow line again
            if (ultrasonic.getDistanceCM() <= 30)
            { 
                robotState = SecondVisit;   //go to the staging platform state
            }
            else{
                chassis.followPath(true);    //if it sees an intersection, turn right?
            }

            break;

        case SecondVisit:
            //define this distance better
            setpoint = STAGING_POS;
            gripper.write(GRIPPER_OPEN);
            //may want to back up some distance from the block, but ultrasoonic is covered by the gripper
            delay(100); //delay for the new block to be placed on
            gripper.write(GRIPPER_CLOSED);
            setpoint = ROOF_25_PICKUP;

            robotState = ReturnRoof;
            break;

    case ReturnRoof:                                   //return to the roof for the deposit of the new panel
    if (ultrasonic.getDistanceCM() <= 30)
        { 
            robotState = FirstVisit;   //go to the staging platform state
        }
        else{
            chassis.followPath(false);    //if it sees an intersection, turn left?
        }
        break;

    case DistanceTest:
    ultrasonic.getDistanceCM();
    Serial.println("im finding the distance");
    break;




        

            
        }
}
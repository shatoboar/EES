/**
 * @file duties.c
 * @author Luca Jungnickel
 * @brief 
 * @version 0.1
 * @date 2022-05-23
 * 
 * @copyright Copyright (c) 2022
 * 
 * Contains all main functions of the robot
 */


#include "ecrobot_interface.h"
#include "duties.h"
#include "motor_helper.h"

void dispense() {
    movesDegrees(NXT_PORT_C, 360, 100);
}

void moveToCamera() {
    movesDegrees(NXT_PORT_A, 90, 50);
}

int getCameraData() {
    return 0;
}

int moveToBox(int numberBox, int currentPosition) {
    int moves = numberBox - currentPosition;
    if (moves > 0) {
        for (int i=0; i < moves; i++) {
            moveOneUnit(1, 100);
        }
    } else if (moves < 0) {
        for (int i=0; i < (-1) * moves; i++) {
            moveOneUnit(-1, 100);
        }
    } else { //already done

    }
    return 0;
}

/**
 * Moves robot one box in the given direction.
 * 
 * @param direction 1 or -1
 */
void moveOneUnit(int direction, int speed) {
    if (direction == -1) {
        movesDegrees(NXT_PORT_B, 180, -speed);
    } else if (direction == 1) {
        movesDegrees(NXT_PORT_B, 180, speed);
    }
}



int dropItem() {
    movesDegrees(NXT_PORT_A, 360, 100);
    return 0;
}
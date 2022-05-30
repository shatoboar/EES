#include "ecrobot_interface.h"

#ifndef MAIN_ROUTINE_H
#define MAIN_ROUTINE_H


#define THROW_STONE_TASK         0
#define MOVE_TO_BOX_TASK         1
#define TMP_MOVE_TASK            2
#define READY_FOR_STONE_TASK     3
#define CALIBRATE_MOVE_LEFT_TASK 4
#define IDLE_TASK                5

#define MOTOR_ASSEMBLY_LINE         NXT_PORT_A
#define MOTOR_MOVE                  NXT_PORT_B
#define SENSOR_CALIBRATION_STONE    NXT_PORT_S1
#define SENSOR_TOUCH_LEFT           NXT_PORT_S2
#define SENSOR_TOUCH_RIGHT          NXT_PORT_S3

//global variables:
static U16 LIGHT_THRESHOLD = 500; //protected by ResourceLightThreshold
/**
 * 1 = forward direction
 * -1 = backward direction
 */
static int DIRECTION = 1;         //protected by ResourceDirection
/**
 * Number of boxes, which are remaining until the correct box is found.
 * Should be positive our 0.
 */
static int BOXES_REMAINING = 3;    //protected by ResourceBoxesRemaining

/**
 * Is the next box, which the robot will go to and drop the stone.
 * -1 if currently there is no next box target.
 * 
 */
static int NEXT_BOX_TARGET = 0; //protected by ResourceNextBoxTarget
/**
 * The next right box is meant by the current box position.
 */
static int CURRENT_BOX = 0; //protected by ResourceCurrentBox

/**
 *  0: ThrowStone
 *  1: MoveToBox
 *  2: TmpMove
 *  3: ReadyForStone
 *  4: IDLE
 *  5: CalibrateMoveLeft
 */
static int MODE = READY_FOR_STONE_TASK;




#endif //MAIN_ROUTINE_H
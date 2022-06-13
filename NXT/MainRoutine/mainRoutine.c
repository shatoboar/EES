/**
 * @file mainRoutine3.c
 * @author Luca Jungnickel
 * @brief 
 * @version 0.1
 * @date 2022-05-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/* eds.c */
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"
#include <stdbool.h>

#include "duties.h"
#include "motor_helper.h"

DeclareCounter(SysTimerCnt);

DeclareTask(EventDispatcherTask);
DeclareTask(LCDTask);
DeclareTask(MainTask);



DeclareResource(ResourceMainRobot);
DeclareResource(ResourceExecuteCommand);


#define THROW_STONE_TASK         0
#define MOVE_TO_BOX_TASK         1
#define TMP_MOVE_TASK            2
#define READY_FOR_STONE_TASK     3
#define THROW_STONE_ON_LINE_TASK 4
#define TAKE_PICTURE_TASK        5
#define CALIBRATE_MOVE_LEFT_TASK 6
#define IDLE_TASK                7

#define MOTOR_ASSEMBLY_LINE         NXT_PORT_A
#define MOTOR_MOVE                  NXT_PORT_B
#define MOTOR_DISPENSER             NXT_PORT_C
#define SENSOR_CALIBRATION_STONE    NXT_PORT_S1
#define SENSOR_TOUCH_RIGHT          NXT_PORT_S2
#define SENSOR_TOUCH_LEFT           NXT_PORT_S3



//---------------ResourceMainRobot-------------------------------------------------------------------
//global variables:
static U16 LIGHT_THRESHOLD = 500;
/**
 * 1 = forward direction (right in direction)
 * -1 = backward direction (left)
 */
static int DIRECTION = 1;
/**
 * Number of boxes, which are remaining until the correct box is found.
 * Should be positive or 0.
 */
static int BOXES_REMAINING = 0;
/**
 * The next right box is meant by the current box position.
 */
static int CURRENT_BOX = 0;

//-----------------ResourceExecuteCommand-----------------------------------------------------------------
/**
 * Is the next box, which the robot will go to and drop the stone.
 * -1 if currently there is no next box target.
 * 
 */
static int NEXT_BOX_TARGET = 0;
//----------------------------------------------------------------------------------
static bool SHOULD_SEND_MESSAGE = false;
static int MESSAGE_SEND_TYPE = -1;
static int MESSAGE_SEND_PAYLOAD = -1;

static bool MESSAGE_RECEIVED = false;
static int MESSAGE_RECEIVE_TYPE = -1;

/**
 *  0: ThrowStone
 *  1: MoveToBox
 *  2: TmpMove
 *  3: ReadyForStone
 *  4: ThrowStoneOnLine
 *  5: TakePicture
 *  6: IDLE
 *  7: CalibrateMoveLeft
 */
static int MODE = CALIBRATE_MOVE_LEFT_TASK; //protected by ResourceMode




/* nxtOSEK hooks */
void ecrobot_device_initialize(void)
{
  nxt_motor_set_speed(NXT_PORT_A, 0, 1);
  nxt_motor_set_speed(NXT_PORT_B, 0, 1);
  ecrobot_set_light_sensor_active(NXT_PORT_S1);


}

void ecrobot_device_terminate(void)
{
  nxt_motor_set_speed(NXT_PORT_A, 0, 1);
  nxt_motor_set_speed(NXT_PORT_B, 0, 1);
  ecrobot_set_light_sensor_inactive(NXT_PORT_S1);
}

/* nxtOSEK hook to be invoked from an ISR in category 2 */
void user_1ms_isr_type2(void)
{
  StatusType ercd;
  ercd = SignalCounter(SysTimerCnt); /* Increment OSEK Alarm Counter */
  if (ercd != E_OK)
  {
    ShutdownOS(ercd);
  }
}


/* EventDispatcher executed every 1ms */
TASK(EventDispatcherTask)
{
    U8 isPressed = 0;
    U8 isPressedRight = 0;

    static bool wasPressedLastTick = false;
    static bool wasPressedRightLastTick = false;

    static bool isOnStartup = true;


    
    //middle big button dispatching
    isPressed = ecrobot_is_ENTER_button_pressed();
    if (isPressed == 1 && !wasPressedLastTick) {

      //calibrate Threshold
      //Poll light data
      U16 light_data = ecrobot_get_light_sensor(NXT_PORT_S1);
      light_data += 50; //for better threshold
      GetResource(ResourceMainRobot);
      LIGHT_THRESHOLD = light_data;
      ReleaseResource(ResourceMainRobot);

    } else if (isPressed == 0 && wasPressedLastTick) {

    }
    wasPressedLastTick = isPressed;


    TerminateTask();
}




/* LCDTask executed every 100ms */
TASK(LCDTask)
{
    display_clear(1);
    //GetResource(ResourceMainRobot);

    display_goto_xy(0, 0);
    display_string("Test");

    display_goto_xy(0, 1);
    display_string("LightThres:");
    display_int(LIGHT_THRESHOLD, 4);

    display_goto_xy(0, 2);
    display_string("Boxesleft:");
    display_int(BOXES_REMAINING, 1);

    display_goto_xy(0, 3);
    display_string("Direction:");
    display_int(DIRECTION, 1);

    display_goto_xy(0, 4);
    display_string("Mode:");
    display_int(MODE, 1);

    //ReleaseResource(ResourceMainRobot);

    //GetResource(ResourceExecuteCommand);
    display_goto_xy(0, 5);
    display_string("NextBox:");
    display_int(NEXT_BOX_TARGET, 1);
    //ReleaseResource(ResourceExecuteCommand);

  //  GetResource(ResourceMainRobot);
    display_goto_xy(0, 6);
    display_string("CurrentBox:");
    display_int(CURRENT_BOX, 1);
//    ReleaseResource(ResourceMainRobot);

    static int test_counter = 0;
    display_goto_xy(0, 7);
    display_string("Tmp Counter:");
    display_unsigned(test_counter, 5);    
    test_counter++;

    display_update();
    

    TerminateTask();
}




TASK(MainTask) {
      while (1)
      {
        /**
         * Should be set to True if MOVE_TO_BOX_TASK, TMP_MOVE_TASK, CALIBRATION_LEFT_STONE is entered.
         * Sets the speed again.
         * Helper guard, so the motor won't be set every tick
         */
        static bool shouldSetSpeed = true;
        
        //for simulating the raspberry pi
        static int boxes[] = {2, 3, 1, 3, 0}; //simulate data from raspberry pi
        static int current_box_index = -1; //-1 is init
        static int max_box = 5;
        static bool should_finish = false;

        switch(MODE) {
          case THROW_STONE_TASK:
            movesDegrees(NXT_PORT_A, 360, 100);
            
            //GetResource(ResourceMainRobot);
            CURRENT_BOX = NEXT_BOX_TARGET;
            //ReleaseResource(ResourceMainRobot);

            //GetResource(ResourceExecuteCommand);
            NEXT_BOX_TARGET = -1;
            //ReleaseResource(ResourceExecuteCommand);

            MODE = READY_FOR_STONE_TASK;
          break;


          case MOVE_TO_BOX_TASK:
            //Move robot until a calibration stone is found
            if (shouldSetSpeed) {
              nxt_motor_set_speed(NXT_PORT_B, 100 * DIRECTION, 1);
              shouldSetSpeed = false;
            }

            //Poll light data
            U16 light_data = ecrobot_get_light_sensor(NXT_PORT_S1);

            //GetResource(ResourceMainRobot);
            U16 light_data_tmp = LIGHT_THRESHOLD;


            if (light_data <= light_data_tmp) {
              //stop motor and decrease boxesRemaining
              nxt_motor_set_speed(NXT_PORT_B, 0, 1);

              BOXES_REMAINING--;

              if (BOXES_REMAINING == 0) {
                MODE = THROW_STONE_TASK;
              } else { //still have to go some more boxes
                shouldSetSpeed = true;
                MODE = TMP_MOVE_TASK;  
              }
            
            }
            //ReleaseResource(ResourceMainRobot);

          break;


          case TMP_MOVE_TASK:
            if (shouldSetSpeed) {     
              movesDegrees(MOTOR_MOVE, 270, 95 * DIRECTION);
              shouldSetSpeed = false;
            }

            shouldSetSpeed = true;
            //GetResource(ResourceMainRobot);
            MODE = MOVE_TO_BOX_TASK;
           // ReleaseResource(ResourceMainRobot);
          break;
          

          case READY_FOR_STONE_TASK:
            
            current_box_index++; //current_box_index is only for simulating PI data and will be removed later
            if (current_box_index >= max_box) {
              current_box_index = -1;
              should_finish = true; //TODO remove
            }
            //set next target
            //GetResource(ResourceExecuteCommand);
            NEXT_BOX_TARGET = boxes[current_box_index]; 
            //ReleaseResource(ResourceExecuteCommand);

            //calculate next position
            //GetResource(ResourceMainRobot);
            BOXES_REMAINING = boxes[current_box_index] - CURRENT_BOX; //should work when boxes remaining are positive
            if (BOXES_REMAINING > 0) {
              DIRECTION = 1;

              shouldSetSpeed = true;
              MODE = TMP_MOVE_TASK;
              
            } else if (BOXES_REMAINING < 0) {
              DIRECTION = -1;
              BOXES_REMAINING = -BOXES_REMAINING; //it's important to negate the boxes remaining, they have to be positive always
              
              shouldSetSpeed = true;
              MODE = TMP_MOVE_TASK;

            } else { //already one correct position, throw stone here
              DIRECTION = 0;
              MODE = THROW_STONE_TASK;
            }
            
            if (should_finish) { //only for simulating PI data, will be removed

              MODE = IDLE_TASK;
            }

            //ReleaseResource(ResourceMainRobot);
          break;

          case CALIBRATE_MOVE_LEFT_TASK:
            if (shouldSetSpeed == true) {
              nxt_motor_set_speed(MOTOR_MOVE, -100, 1);
              shouldSetSpeed = false;
            }
            if (ecrobot_get_touch_sensor(SENSOR_TOUCH_LEFT)) {
              nxt_motor_set_speed(MOTOR_MOVE, 0, 1);
              MODE = READY_FOR_STONE_TASK;
            }
          break;

          case IDLE_TASK:
            nxt_motor_set_speed(MOTOR_MOVE, 0, 1);
            nxt_motor_set_speed(MOTOR_ASSEMBLY_LINE, 0, 1);
          break;
        }
      }

      TerminateTask();
    }
/**
 * @file mainRoutine.c
 * @author Luca Jungnickel
 * 
 * @date 2022-06-13
 * 
 */
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


#define THROW_STONE_TASK            0
#define MOVE_TO_BOX_TASK            1
#define TMP_MOVE_TASK               2
#define READY_FOR_STONE_TASK        3
#define CALC_NEXT_BOX_POSITION_TASK 4
#define THROW_STONE_ON_LINE_TASK    5
#define TAKE_PICTURE_TASK           6
#define CALIBRATE_MOVE_LEFT_TASK    7
#define IDLE_TASK                   8

#define MOTOR_ASSEMBLY_LINE         NXT_PORT_A
#define MOTOR_MOVE                  NXT_PORT_B
#define MOTOR_DISPENSER             NXT_PORT_C
#define SENSOR_CALIBRATION_STONE    NXT_PORT_S1
#define SENSOR_TOUCH_RIGHT          NXT_PORT_S2
#define SENSOR_TOUCH_LEFT           NXT_PORT_S3



//---------------ResourceMainRobot-------------------------------------------------------------------
//global variables:
/**
 * Threshold for the light sensor. Could be calibrated in the future, now it's hardcoded.
 * 
 */
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

/**
 * Current mode of state machine.
 * 
 * see modes under #DEFINE
 */
static int MODE = CALIBRATE_MOVE_LEFT_TASK;

//-----------------ResourceExecuteCommand-----------------------------------------------------------------
/**
 * Is the next box, which the robot will go to and drop the stone.
 * -1 if currently there is no next box target.
 * 
 */
static int NEXT_BOX_TARGET = -1;







/* Standard NXT hooks */
void ecrobot_device_initialize(void)
{
  //set motor speed to 0
  nxt_motor_set_speed(MOTOR_ASSEMBLY_LINE, 0, 1);
  nxt_motor_set_speed(MOTOR_DISPENSER, 0, 1);
  nxt_motor_set_speed(MOTOR_MOVE, 0, 1);
  
  ecrobot_set_light_sensor_active(SENSOR_CALIBRATION_STONE);
}

void ecrobot_device_terminate(void)
{
  //set motor speed to 0
  nxt_motor_set_speed(MOTOR_ASSEMBLY_LINE, 0, 1);
  nxt_motor_set_speed(MOTOR_DISPENSER, 0, 1);
  nxt_motor_set_speed(MOTOR_MOVE, 0, 1);

  ecrobot_set_light_sensor_inactive(SENSOR_CALIBRATION_STONE);
}

void user_1ms_isr_type2(void)
{
  StatusType ercd;
  ercd = SignalCounter(SysTimerCnt); /* Increment OSEK Alarm Counter */
  if (ercd != E_OK)
  {
    ShutdownOS(ercd);
  }
}
/* End Standard NXT hooks */



/* EventDispatcher executed every 1ms */
TASK(EventDispatcherTask)
{
    U8 isPressed = 0;

    static bool wasPressedLastTick = false;

    
    //middle big button dispatching
    isPressed = ecrobot_is_ENTER_button_pressed();
    //prototype of calibrating the light sensor
    if (isPressed == 1 && !wasPressedLastTick) { //TODO: maybe remove?
      
      //calibrate Threshold
      //Poll light data
      U16 light_data = ecrobot_get_light_sensor(SENSOR_CALIBRATION_STONE);
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

    display_goto_xy(0, 0);
    display_string("HELLO");

    GetResource(ResourceMainRobot);
    display_goto_xy(0, 1);
    display_string("LightThres:");
    display_int(LIGHT_THRESHOLD, 4);


    display_goto_xy(0, 2);
    display_string("Boxesleft:");
    display_int(BOXES_REMAINING, 2);

    display_goto_xy(0, 3);
    display_string("Direction:");
    display_int(DIRECTION, 1);

    display_goto_xy(0, 4);
    display_string("Mode:");
    display_int(MODE, 1);

    display_goto_xy(0, 6);
    display_string("CurrentBox:");
    display_int(CURRENT_BOX, 1);
    ReleaseResource(ResourceMainRobot);


    GetResource(ResourceExecuteCommand);
    display_goto_xy(0, 5);
    display_string("NextBox:");
    display_int(NEXT_BOX_TARGET, 1);
    ReleaseResource(ResourceExecuteCommand);

    static int test_counter = 0;
    display_goto_xy(0, 7);
    display_string("Test Counter:");
    display_unsigned(test_counter, 5);    
    test_counter++;

    display_update();
    

    TerminateTask();
}




TASK(MainTask) {
      while (1)
      {
        /**
         * Sets the speed again.
         * Helper guard, so the motor won't be set every tick
         */
        static bool shouldSetSpeed = true;
        
        //for simulating the raspberry pi
        static int boxes[] = {2, 3, 1, 3, 0}; //simulate data from raspberry pi TODO remove
        static int current_box_index = -1; //-1 is init TODO remove
        static int max_box = 5; //TODO remove maybe
        static bool should_finish = false; //TODO remove

        switch(MODE) {
          case THROW_STONE_TASK:
            nxt_motor_set_speed(MOTOR_MOVE, 0, 1); //deactive driving
            movesDegrees(MOTOR_ASSEMBLY_LINE, 360, 100);
            
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
              nxt_motor_set_speed(MOTOR_MOVE, 100 * DIRECTION, 1);
              shouldSetSpeed = false;
            }

            //Poll light data
            U16 light_data = ecrobot_get_light_sensor(SENSOR_CALIBRATION_STONE);

            //GetResource(ResourceMainRobot);
            U16 light_threshold_tmp = LIGHT_THRESHOLD;


            if (light_data <= light_threshold_tmp) {
              //stop motor and decrease boxesRemaining
              nxt_motor_set_speed(MOTOR_MOVE, 0, 1);

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



          case CALC_NEXT_BOX_POSITION_TASK:
            //calculate next position
            //GetResource(ResourceMainRobot);
            BOXES_REMAINING = NEXT_BOX_TARGET - CURRENT_BOX; //should work when boxes remaining are positive
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
            
            //set motor speed to 0
            nxt_motor_set_speed(MOTOR_ASSEMBLY_LINE, 0, 1);
            nxt_motor_set_speed(MOTOR_DISPENSER, 0, 1);
            nxt_motor_set_speed(MOTOR_MOVE, 0, 1);

            //TO DO wait until next stone signal from bluetooth module is coming

            //Throw stone the assembly line
            MODE = THROW_STONE_ON_LINE_TASK;
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

          case THROW_STONE_ON_LINE_TASK:
            //move dispenser and drop stone
            movesDegrees(MOTOR_DISPENSER, 360, 100);
            
            //TODO: Send signal to PI to send picture
            
            MODE = TAKE_PICTURE_TASK;
          break;

          case TAKE_PICTURE_TASK:
            
            //TODO wait until signal from PI for correct box info
            
            //MOCK SIMULATION
            current_box_index++; //current_box_index is only for simulating PI data and will be removed later
            if (current_box_index >= max_box) {
              current_box_index = -1;
              should_finish = true; //TODO remove

            }
            //set next target
            //GetResource(ResourceExecuteCommand);
            //TODO remove !should_finish
            if (!should_finish) NEXT_BOX_TARGET = boxes[current_box_index]; 
            //ReleaseResource(ResourceExecuteCommand);

            MODE = CALC_NEXT_BOX_POSITION_TASK;

            if (should_finish) { //TODO only for simulating PI data, will be removed
              MODE = IDLE_TASK;
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
/**
 * @file mainTask.c
 * @author Luca Jungnickel
 * @brief 
 * @version 0.1
 * @date 2022-05-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "mainTask.h"
#include "mainRoutine3.h"
#include "ecrobot_interface.h"
#include <stdbool.h>
#include "motor_helper.h"
#include "kernel.h"
#include "kernel_id.h"


TASK(MainTask) {
      while (1)
      {
        /**
         * Should be set to True if MOVE_TO_BOX_TASK or TMP_MOVE_TASK is entered.
         * Sets the speed again.
         * Helper guard, so the motor won't be set every tick
         */
        static bool shouldSetSpeed = true;
        
        //for simulating the raspberry pi
        static int boxes[] = {1, 2, 3, 0, 1, 2, 3, 0, 3, 0, 3, 0}; //simulate data from raspberry pi
        static int current_box_index = -1; //-1 is init
        static int max_box = 12;

        switch(MODE) {
          case THROW_STONE_TASK:
            movesDegrees(NXT_PORT_A, 360, 100);
            
            GetResource(ResourceCurrentBox);
            GetResource(ResourceNextBoxTarget);
            CURRENT_BOX = NEXT_BOX_TARGET;
            NEXT_BOX_TARGET = -1;
            ReleaseResource(ResourceCurrentBox);
            ReleaseResource(ResourceNextBoxTarget);

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
            
            GetResource(ResourceLightThreshold);
            U16 light_data_tmp = LIGHT_THRESHOLD;
            ReleaseResource(ResourceLightThreshold);

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
          break;


          case TMP_MOVE_TASK:
            if (shouldSetSpeed) {     
              movesDegrees(MOTOR_MOVE, 270, 95 * DIRECTION);
              shouldSetSpeed = false;
            }

            shouldSetSpeed = true;
            MODE = MOVE_TO_BOX_TASK;
          break;
          

          case READY_FOR_STONE_TASK:
            
            current_box_index++;
            if (current_box_index > max_box) {
              current_box_index = 0;
              MODE = IDLE_TASK; //TODO remove
            }
            //set next target
            GetResource(ResourceNextBoxTarget);
            NEXT_BOX_TARGET = boxes[current_box_index]; 
            ReleaseResource(ResourceNextBoxTarget);

            //calculate next position
            GetResource(DIRECTION);
            GetResource(BOXES_REMAINING);
            BOXES_REMAINING = boxes[current_box_index] - CURRENT_BOX; //should work when boxes remaining are positive
            if (BOXES_REMAINING > 0) {
              DIRECTION = 1;

              shouldSetSpeed = true;
              if (MODE != IDLE_TASK) { //TODO remove
                MODE = TMP_MOVE_TASK;
              }
            } else if (BOXES_REMAINING < 0) {
              DIRECTION = -1;
              BOXES_REMAINING = -BOXES_REMAINING; //it's important to negate the boxes remaining, they have to be positive always
              
              shouldSetSpeed = true;
              if (MODE != IDLE_TASK) { //TODO remove
                MODE = TMP_MOVE_TASK;
              }
            } else { //already one correct position, throw stone here
              DIRECTION = 0;
              if (MODE != IDLE_TASK) { //TODO remove
                MODE = THROW_STONE_TASK;
              }
            }
            ReleaseResource(DIRECTION);
            ReleaseResource(BOXES_REMAINING);
            
          break;

          case CALIBRATE_MOVE_LEFT_TASK:
            if (shouldSetSpeed == true) {
              //nxt_motor_set_speed();
              shouldSetSpeed = false;
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
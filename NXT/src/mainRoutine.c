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
#include "bluetooth_module.h"

DeclareCounter(SysTimerCnt);

DeclareTask(EventDispatcherTask);
DeclareTask(LCDTask);
DeclareTask(MainTask);

DeclareResource(ResourceMainRobot);
DeclareResource(ResourceExecuteCommand);

#define RUNTIME_CONNECTION

#define RESET_TASK                            0
#define THROW_STONE_TASK                      1
#define MOVE_TO_BOX_TASK                      2
#define TMP_MOVE_TASK                         3
#define READY_FOR_STONE_TASK                  4
#define CALC_NEXT_BOX_POSITION_TASK           5
#define THROW_STONE_ON_LINE_TASK              6
#define GO_TO_DISPENSER_TASK                  7
#define TAKE_PICTURE_TASK                     8
#define CALIBRATE_MOVE_LEFT_TASK              9
#define ERROR_LIGHT                           10
/**
 * Starts the error correction calibrate phase.
 * The robot will drive to the right side until a bumber stone was hit, the to the left and count the stones.
 * If the number of counted calibration stones are correct and the left bumber stone was hit, everything is ok again.
 * Timeout of 4s in which a calibration has to be found, otherwise it will go into the ERROR_FATAL state.
 */
#define ERROR_CORRECTION_CALIBRATE_START            11  
#define ERROR_CORRECTION_CALIBRATE_RIGHT            12 //entered after error_correction_calibrate_start, will enter _calibrate_left
#define ERROR_CORRECTION_CALIBRATE_LEFT             13 //drives to the left and counts the stones
#define ERROR_CORRECTION_CALIBRATE_LEFT_TMP         14 //tmp move if a calibration stone was detected
#define ERROR_CORRECTION_CALIBRATE_LEFT_STONE_BUMP  15 //stones counted are correct, now a left bumber stone is expected
#define ERROR_FATAL                                 16
#define IDLE_TASK                                   17

#define MOTOR_ASSEMBLY_LINE         NXT_PORT_A
#define MOTOR_MOVE                  NXT_PORT_B
#define MOTOR_DISPENSER             NXT_PORT_C
#define SENSOR_CALIBRATION_STONE    NXT_PORT_S1
#define SENSOR_TOUCH_RIGHT          NXT_PORT_S2
#define SENSOR_TOUCH_LEFT           NXT_PORT_S3

#define DEGREES_DISPENSER           250


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
 *  Number of boxes on the line. Now hardcoded, could be counted in the future by the robot
 */
static int NUMBER_OF_BOXES = 7;
/**
 * 
 * FLAG, if a touch sensor is activated and should be touched in the next time.
 * Important for safety requirements, if a touch sensor is touched while not being activated an error will be thrown.
 */
static bool TOUCH_SENSOR_LEFT_ACTIVATED = false;
static bool TOUCH_SENSOR_RIGHT_ACTIVATED = false;

/**
 * Counter for counting boxes if robot is in error state.
 * 
 */
static int ERROR_COUNTED_BOXES = 0;

static bool IS_IN_ERROR_MODE = false;

/**
 * Current mode of state machine.
 * 
 * see modes under #DEFINE
 */
static int MODE = RESET_TASK;

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
    #ifndef RUNTIME_CONNECTION
        ecrobot_init_bt_slave("1234");
    #endif

    ecrobot_set_light_sensor_active(SENSOR_CALIBRATION_STONE);
}

void ecrobot_device_terminate(void)
{
    //set motor speed to 0
    nxt_motor_set_speed(MOTOR_ASSEMBLY_LINE, 0, 1);
    nxt_motor_set_speed(MOTOR_DISPENSER, 0, 1);
    nxt_motor_set_speed(MOTOR_MOVE, 0, 1);
    ecrobot_term_bt_connection();

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
    if (isPressed == 1 && !wasPressedLastTick) {

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
    display_string("BLT Debug:");
    display_int(bluetooth_get_debug_int(), 4);


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
        static bool inited = false;

        static bool ok; //helper flag
        static U8 box; 
        
        bluetooth_poll();
        ok = false;

        switch(MODE) {
            case RESET_TASK:
                movesDegrees(MOTOR_DISPENSER, DEGREES_DISPENSER, 70); //reset pusher to idle
                MODE = CALIBRATE_MOVE_LEFT_TASK;
                break;
            case THROW_STONE_TASK:
                nxt_motor_set_speed(MOTOR_MOVE, 0, 1); //deactive driving

                movesDegrees(MOTOR_ASSEMBLY_LINE, 350, 100);

                GetResource(ResourceExecuteCommand);
                GetResource(ResourceMainRobot);
                CURRENT_BOX = NEXT_BOX_TARGET;
                ReleaseResource(ResourceMainRobot);

                NEXT_BOX_TARGET = -1;
                ReleaseResource(ResourceExecuteCommand);

                //send signal to PI that NXT is ready
                ok = bluetooth_send_stone_sorted_signal();
                MODE = GO_TO_DISPENSER_TASK;

                break;



            case MOVE_TO_BOX_TASK:
                GetResource(ResourceMainRobot);

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
                ReleaseResource(ResourceMainRobot);
                break;



            case CALC_NEXT_BOX_POSITION_TASK:
                //calculate next position
                GetResource(ResourceMainRobot);
                GetResource(ResourceExecuteCommand);

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
            
                ReleaseResource(ResourceMainRobot);
                ReleaseResource(ResourceExecuteCommand);
                break;




          case GO_TO_DISPENSER_TASK:
            TOUCH_SENSOR_LEFT_ACTIVATED = true;
            
            nxt_motor_set_speed(MOTOR_MOVE, -100, 1);

            if (ecrobot_get_touch_sensor(SENSOR_TOUCH_LEFT)) {
              nxt_motor_set_speed(MOTOR_MOVE, 0, 1);
              movesDegrees(MOTOR_MOVE, 75, 100); //move back
              TOUCH_SENSOR_LEFT_ACTIVATED = false;
              CURRENT_BOX = 0;
              MODE = READY_FOR_STONE_TASK;
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

                //set motor speed to 0
                nxt_motor_set_speed(MOTOR_ASSEMBLY_LINE, 0, 1);
                nxt_motor_set_speed(MOTOR_DISPENSER, 0, 1);
                nxt_motor_set_speed(MOTOR_MOVE, 0, 1);
                
                if(!inited){
                    bluetooth_init(NUMBER_OF_BOXES);
                    inited = true;
                }
                

                //wait until next stone signal from bluetooth module is coming
                bluetooth_rcv_next_stone_signal();

                //Throw stone the assembly line
                MODE = THROW_STONE_ON_LINE_TASK;

                break;

            case CALIBRATE_MOVE_LEFT_TASK:


                TOUCH_SENSOR_LEFT_ACTIVATED = true;


                if (shouldSetSpeed == true) {
                    nxt_motor_set_speed(MOTOR_MOVE, -100, 1);
                    shouldSetSpeed = false;
                }
                if (ecrobot_get_touch_sensor(SENSOR_TOUCH_LEFT)) {
                    //nxt_motor_set_speed(MOTOR_MOVE, 0, 1);
                    movesDegrees(MOTOR_MOVE, 70, 100);
                    TOUCH_SENSOR_LEFT_ACTIVATED = false;
                    MODE = READY_FOR_STONE_TASK;
                }
                break;

            case THROW_STONE_ON_LINE_TASK:
                //move dispenser and drop stone
                movesDegrees(MOTOR_DISPENSER, DEGREES_DISPENSER, -85); //push
                movesDegrees(MOTOR_DISPENSER, DEGREES_DISPENSER, 85); //and retreat
                
                //TODO RICHARDT: hier einf??gen, dass sich Assembly Line leicht nach vorne bewegt und Stein besser im Zentrum der Kamera ist
                //send signal to PI, to take a picture
                ok = bluetooth_send_next_picture_signal();

                MODE = TAKE_PICTURE_TASK;

                break;

            case TAKE_PICTURE_TASK:

                //wait until signal from PI for correct box info
                box = bluetooth_rcv_sort_in_box_signal();
                box = box - 1; //important, because in the network protocol we start counting with number one


                GetResource(ResourceExecuteCommand);
                
                //set next target
                NEXT_BOX_TARGET = box;

                ReleaseResource(ResourceExecuteCommand);

                MODE = CALC_NEXT_BOX_POSITION_TASK;

                break;

                //------------------------------------ERROR HANDLING------------------------------------
            case ERROR_LIGHT:
                IS_IN_ERROR_MODE = true;
                MODE = ERROR_CORRECTION_CALIBRATE_START;
                break;

            case ERROR_CORRECTION_CALIBRATE_START:
                //try to calibrate again with 10 seconds timeout
                //drive to the right until the bumber stone is hit, then drive to the left and count the stones

                //reset
                nxt_motor_set_speed(MOTOR_ASSEMBLY_LINE, 0, 1);
                nxt_motor_set_speed(MOTOR_DISPENSER, 0, 1);
                nxt_motor_set_speed(MOTOR_MOVE, 0, 1);
                ERROR_COUNTED_BOXES = 0;

                //drive to the right
                movesDegrees(MOTOR_MOVE, 90, 100); //a little bit outside of the touch sensor
                nxt_motor_set_speed(MOTOR_MOVE, 100, 1);

                MODE = ERROR_CORRECTION_CALIBRATE_RIGHT;
                break;

            case ERROR_CORRECTION_CALIBRATE_RIGHT:
                //wait until the right bumber stone is hit
                if (ecrobot_get_touch_sensor(SENSOR_TOUCH_RIGHT)) {
                    //start counting boxes

                    U16 data = ecrobot_get_light_sensor(SENSOR_CALIBRATION_STONE);
                    if (data <= LIGHT_THRESHOLD) { //over a stone
                        MODE = ERROR_CORRECTION_CALIBRATE_LEFT_TMP;
                        ERROR_COUNTED_BOXES++;
                    } else { //not over a stone
                        MODE = ERROR_CORRECTION_CALIBRATE_LEFT;
                    }

                    //drive to the left
                    movesDegrees(MOTOR_MOVE, 90, -100);
                    nxt_motor_set_speed(MOTOR_MOVE, -100, 1);
                }

                break;

            case ERROR_CORRECTION_CALIBRATE_LEFT:
                if (ERROR_COUNTED_BOXES == NUMBER_OF_BOXES) {
                    nxt_motor_set_speed(MOTOR_MOVE, -100, 1); //drive to left
                    MODE = ERROR_CORRECTION_CALIBRATE_LEFT_STONE_BUMP;
                } else {
                    U16 light = ecrobot_get_light_sensor(SENSOR_CALIBRATION_STONE);
                    U8 touchedLeft = ecrobot_get_touch_sensor(SENSOR_TOUCH_LEFT);
                    U8 touchedRight = ecrobot_get_touch_sensor(SENSOR_TOUCH_RIGHT);
                    if (!touchedLeft && !touchedRight) {
                        if (light <= LIGHT_THRESHOLD) { //over a stone
                            MODE = ERROR_CORRECTION_CALIBRATE_LEFT_TMP;
                            ERROR_COUNTED_BOXES++;
                        } else { //not over a stone

                        }
                    } else { //touched, error happened
                        MODE = ERROR_FATAL;
                    }
                }
                break;

            case ERROR_CORRECTION_CALIBRATE_LEFT_TMP:
                if (ERROR_COUNTED_BOXES == NUMBER_OF_BOXES) {
                    movesDegrees(MOTOR_MOVE, 270, -95); //get outside the calibration stone area

                    MODE = ERROR_CORRECTION_CALIBRATE_LEFT_STONE_BUMP;
                } else {
                    U8 touchedLeft = ecrobot_get_touch_sensor(SENSOR_TOUCH_LEFT);
                    U8 touchedRight = ecrobot_get_touch_sensor(SENSOR_TOUCH_RIGHT);
                    if (!touchedLeft && !touchedRight) {
                        //get outside the calibration stone area
                        movesDegrees(MOTOR_MOVE, 270, -95);
                        nxt_motor_set_speed(MOTOR_MOVE, -100, 1); //drive to left
                        MODE = ERROR_CORRECTION_CALIBRATE_LEFT; //back to normal counting mode
                    } else {
                        MODE = ERROR_FATAL;
                    }
                }
                break;

            case ERROR_CORRECTION_CALIBRATE_LEFT_STONE_BUMP:
                //robot is still driving from other states
                //expecting to get a bumber stone hit and not a calibration stone again
                //Poll light data
                MODE = ERROR_CORRECTION_CALIBRATE_LEFT_STONE_BUMP; //no meaning behind it. Otherwise the code will not compile, because a case shouldn't be followed by U16 statement.....

                U16 light_data2 = ecrobot_get_light_sensor(SENSOR_CALIBRATION_STONE);
                if (light_data2 <= LIGHT_THRESHOLD) { //over a stone
                    MODE = ERROR_FATAL; //throw fatal error
                } else {
                    if (ecrobot_get_touch_sensor(SENSOR_TOUCH_LEFT)) {
                        nxt_motor_set_speed(MOTOR_MOVE, 0, 1);
                        movesDegrees(MOTOR_MOVE, 90, 100); //a little bit to the right
                        TOUCH_SENSOR_LEFT_ACTIVATED = false;
                        IS_IN_ERROR_MODE = false;
                        MODE = READY_FOR_STONE_TASK; //then everything is fine again
                    }
                }
                break;


            case ERROR_FATAL:
                nxt_motor_set_speed(MOTOR_MOVE, 0, 1);
                nxt_motor_set_speed(MOTOR_ASSEMBLY_LINE, 0, 1);
                nxt_motor_set_speed(MOTOR_DISPENSER, 0, 1);
                MODE = ERROR_FATAL;
                break;

            case IDLE_TASK:
                nxt_motor_set_speed(MOTOR_MOVE, 0, 1);
                nxt_motor_set_speed(MOTOR_ASSEMBLY_LINE, 0, 1);
                nxt_motor_set_speed(MOTOR_DISPENSER, 0, 1);
                break;
        }
        //this code is always executed:

        //check if touch sensor is touched invalidly
        if (!IS_IN_ERROR_MODE) {
            if (ecrobot_get_touch_sensor(SENSOR_TOUCH_LEFT) && !TOUCH_SENSOR_LEFT_ACTIVATED) {
                MODE = ERROR_LIGHT;
            } else if (ecrobot_get_touch_sensor(SENSOR_TOUCH_RIGHT) && ! TOUCH_SENSOR_RIGHT_ACTIVATED) {
                MODE = ERROR_LIGHT;
            }
        }

    }

    TerminateTask();
}

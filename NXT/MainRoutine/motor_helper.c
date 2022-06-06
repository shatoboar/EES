
/**
 * @file motor_helper.c
 * @author Luca Jungnickel
 * @brief 
 * @version 0.1
 * @date 2022-05-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "ecrobot_interface.h"
#include "motor_helper.h"

/**
 * 
 * Starts motor and tries to move it only the specified degrees.
 * Most likely the motor will move more than the given degrees, the additional degrees will be returned
 * 
 * @param motor_port like NXT_PORT_A
 * @param degrees degrees, like 360 is one complete rotation
 * @return int The additional degrees the motor was moved
 */
int movesDegrees(U32 motor_port, int degrees, int speed) {

    int motor_count = 0;
    nxt_motor_set_count(motor_port, 0);
    int offset = nxt_motor_get_count(motor_port);

    motor_count = nxt_motor_get_count(motor_port);
    nxt_motor_set_speed(motor_port, speed, 1);
    if (speed > 0) {
        while (motor_count < degrees) {
            motor_count = nxt_motor_get_count(motor_port);
        }
    } else if (speed < 0) {
        while (-motor_count < degrees) {
            motor_count = nxt_motor_get_count(motor_port);            
        }
    }

    nxt_motor_set_speed(motor_port, 0, 1); //off
    int count = nxt_motor_get_count(motor_port);
    nxt_motor_set_count(motor_port, 0); //reset motor count

    return count - degrees;
}
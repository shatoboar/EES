#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <stdbool.h>
#include "ecrobot_interface.h"

/**
 * @brief Will be called once when the NXT is initialising.
 * 
 * @return true if everything worked
 * @return false if an error ocurred
 */
bool bluetooth_init();

/**
 * @brief Will be always called in main loop by routine.
 * Could also be called before bluetooth_init()!
 * 
 * @return true if everything worked
 * @return false if an error ocurred
 */
bool bluetooth_poll();

/**
 * @brief Waits and gets a signal via bluetooth (and returns an ACK) from PI that the NXT 
 * should dispense a stone onto the assembly line.
 * 
 * Blocking function.
 * 
 * @return true if everything worked
 * @return false if receiving correct signal didn't work
 */
bool bluetooth_rcv_next_stone_signal();

/**
 * @brief Sends a signal via bluetooth (and wait for ACK) from NXT to PI that the NXT is 
 * ready and the PI should take a picture.
 * 
 * Non-blocking function.
 * 
 * @return true if everything worked
 * @return false if receiving correct signal didn't work
 */
bool bluetooth_send_next_picture_signal();

/**
 * @brief Waits and gets a signal via bluetooth (and returns an ACK) from PI that the NXT 
 * should sort the next stone
 * into the correct box.
 * 
 * Blocking function.
 * 
 * @return box index
 */
U8 bluetooth_rcv_sort_in_box_signal();

/**
 * @brief Sends a signal via bluetooth (and wait for ACK) from NXT to PI that the item has been 
 * sorted and the NXT is ready again.
 * 
 * @return true if everything worked
 * @return false if receiving correct signal didn't work
 */
bool bluetooth_send_stone_sorted_signal();

#endif //BLUETOOTH_H
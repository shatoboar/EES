#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <stdbool.h>
#include "ecrobot_interface.h"

#define RUNTIME_CONNECTION

// send when error occurs
#define ERROR           255

// sent as acknowledgement
#define ACK             1

// sent during initialization
#define INIT            2

// pi -> nxt
//      deploy a new stone onto the line
//      Messagecontent: empty
// nxt -> pi 
//      Deploy item done
//      Messagecontent: empty
#define DEPLOY_ITEM     3

// pi -> nxt
//      sort in the stone
//      Messagecontent: bucketnumber
// nxt -> pi
//      stone is sorted in
//      Messagecontent: empty if done
#define PREDICTED_BNR   4 

// throw stone on line  | pi -> nxt
// pi take picture      | nxt -> pi
// get box number       | pi -> nxt
// nxt ready            | nxt -> pi

/**
 * @brief Will be called during initialization
 * @param numOfBuckets, sends how many buckets we have.
 *
 * ATTENTION: This needs to be implemented in a loop.
 * Call this function inside ecrobot_device_initialize()
 */
bool bluetooth_init(int numOfBuckets);

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

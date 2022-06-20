#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <stdbool.h>

/**
 * @brief Will be called once when the NXT is initialising.
 * 
 * @return true if everything worked
 * @return false if an error ocurred
 */
bool bluetooth_init();

/**
 * @brief Waits and gets a signal via bluetooth from PI that the NXT should dispense a stone 
 * onto the assembly line.
 * 
 * Blocking function.
 * 
 * @return true if everything worked
 * @return false if receiving correct signal didn't work
 */
bool bluetooth_rcv_next_stone_signal();

bool bluetooth_send_next_picture_signal();

bool bluetooth_rcv_sort_in_box_signal();

bool bluetooth_send_stone_sorted_signal();

#endif //BLUETOOTH_H
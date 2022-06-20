
#include <stdbool.h>
#include "ecrobot_interface.h"

bool bluetooth_init() {
    return true;
}

bool bluetooth_poll() {
    return true;
}

bool bluetooth_rcv_next_stone_signal() {
    return true;
}

bool bluetooth_send_next_picture_signal() {
    return true;
}

U8 bluetooth_rcv_sort_in_box_signal() {
    return 0;
}

bool bluetooth_send_stone_sorted_signal() {
    return true;
}
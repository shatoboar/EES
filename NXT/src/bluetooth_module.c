#include <string.h> 
#include <stdbool.h> 
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"
#include "bluetooth_module.h"

bool verify(U8 msg, U8 checksum);
void marshal(U8* buf, U8 msg_type, U8 msg_payload);
bool unmarshal(U8 *buf, int buf_len);
bool bluetooth_send(int cmd) ;

bool verify(U8 msg, U8 checksum) 
{
    return (msg == ~checksum) == 0 && msg != 0;
}

bool unmarshal(U8 *buf, int buf_len) 
{
    U8 message_type = buf[0];	
    if (!verify(message_type, buf[2])) 
        return false; 

    if (buf[1] != 0) {
        U8 message_payload = buf[1];
        if (!verify(message_payload, buf[3]))
            return false; 
    }

    return true;
}

// Beware of the case that the messages being sent could be 0
void marshal(U8* buf, U8 msg_type, U8 msg_payload) 
{
    buf[0] = msg_type;
    buf[2] = ~msg_type;

    buf[1] = msg_payload;
    buf[3] = ~msg_payload;
}


bool bluetooth_poll() {
#ifdef RUNTIME_CONNECTION
    ecrobot_init_bt_slave("1234");
#endif

    return ecrobot_get_bt_status() == BT_STREAM; 
}

/*bool send(int cmd) { */
/*    static SINT bt_status = BT_NO_INIT; */
/*    U8 bt_recv_buf[4]; */
/*    U8 bt_send_buf[4]; */

/*    while(1) */ 
/*    { */
/*#ifdef RUNTIME_CONNECTION */
/*        ecrobot_init_bt_slave("1234"); */
/*#endif */
/*        if (ecrobot_get_bt_status() == BT_STREAM && bt_status != BT_STREAM) { */
/*            marshal(bt_send_buf, cmd, 0); */
/*            ecrobot_send_bt(bt_send_buf, 0, 4); */

/*            int read = 0; */
/*            while(read == 0){ */
/*                read = ecrobot_read_bt(bt_recv_buf, 0, 4); */
/*                systick_wait_ms(500); */
/*            } */

/*            bool ok = unmarshal(bt_recv_buf, read); */
/*            if (!ok) { */
/*                marshal(bt_send_buf, ERROR, 0); */
/*                ecrobot_send_bt(bt_send_buf, 0, 4); */
/*                continue; */
/*            } */ 

/*            return true; */
/*        } */
/*    } */
/*} */

/*/1* */
/* * @brief This is a wrapper function for reading that sends back an ack under the hood. */
/* * */
/* * @param this is the expected command at thsi current stage */
/* * @returns 0 if all right, else returns whatever is expected at the current stage */
/* * */
/* * *1/ */
/*U8 recv(int expectedCmd) { */
/*    static SINT bt_status = BT_NO_INIT; */
/*    U8 bt_recv_buf[4]; */
/*    U8 bt_send_buf[4]; */

/*    while(1) */ 
/*    { */
/*#ifdef RUNTIME_CONNECTION */
/*        ecrobot_init_bt_slave("1234"); */
/*#endif */
/*        if (ecrobot_get_bt_status() == BT_STREAM && bt_status != BT_STREAM) { */
/*            int read = 0; */
/*            while(read == 0){ */
/*                read = ecrobot_read_bt(bt_recv_buf, 0, 4); */
/*                systick_wait_ms(500); */
/*            } */

/*            // if we don't receive a correct message then we restart the loop */
/*            bool ok = unmarshal(bt_recv_buf, read); */
/*            if (!ok) { */
/*                marshal(bt_send_buf, ERROR, 0); */
/*                ecrobot_send_bt(bt_send_buf, 0, 4); */
/*                continue; */
/*            } */ 

/*            U8 message_type = bt_recv_buf[0]; */
/*            U8 message_payload  = bt_recv_buf[1]; */

/*            // received message type wasn't what we expected send back error */
/*            if (message_type != cmd) { */ 
/*                marshal(bt_send_buf, ERROR, 0); */
/*                ecrobot_send_bt(bt_send_buf, 0, 4); */
/*                continue; */
/*            } */

/*            marshal(bt_send_buf, ACK, 0); */
/*            ecrobot_send_bt(bt_send_buf, 0, 4); */
/*            return message_payload; */
/*        } */
/*    } */
/*} */

// ATTENTION: this needs to be implemented in a loop 
// call this function inside ecrobot_device_initialize
bool bluetooth_init(int numOfBuckets) {
    static SINT bt_status = BT_NO_INIT;
    U8 bt_recv_buf[4];
    U8 bt_send_buf[4];

    while(1) 
    {
#ifdef RUNTIME_CONNECTION
        ecrobot_init_bt_slave("1234");
#endif
        if (ecrobot_get_bt_status() == BT_STREAM && bt_status != BT_STREAM) {
            int read = 0;
            while(read == 0){
                read = ecrobot_read_bt(bt_recv_buf, 0, 4);
                systick_wait_ms(500);
            }

            // if we don't receive a correct message then we restart the loop
            bool ok = unmarshal(bt_recv_buf, read);
            if (!ok) {
                marshal(bt_send_buf, ERROR, 0);
                ecrobot_send_bt(bt_send_buf, 0, 4);
                continue;
            } 

            marshal(bt_send_buf, ACK, 0);
            ecrobot_send_bt(bt_send_buf, 0, 4);

            systick_wait_ms(500);

            marshal(bt_send_buf, INIT, numOfBuckets);
            ecrobot_send_bt(bt_send_buf, 0, 4);
            break;
        }
    }
}

bool bluetooth_rcv_next_stone_signal() {
    int read = 0;
    U8 bt_recv_buf[4];
    U8 bt_send_buf[4];

    while (1)
    {
        while(read == 0){
            read = ecrobot_read_bt(bt_recv_buf, 0, 4);
            systick_wait_ms(500);
        }

        bool ok = unmarshal(bt_recv_buf, read);
        if (!ok) {
            marshal(bt_send_buf, ERROR, 0);
            ecrobot_send_bt(bt_send_buf, 0, 4);
            return false;
        } 
    }

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

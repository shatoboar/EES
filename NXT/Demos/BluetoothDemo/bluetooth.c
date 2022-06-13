/* /1* btslave.c *1/ */ 
#include <string.h> 
#include <stdbool.h> 
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

/* OSEK declarations */
DeclareCounter(SysTimerCnt);
DeclareTask(IdleTask);

DeclareResource(BluetoothResource);

/* below macro enables run-time Bluetooth connection */
#define RUNTIME_CONNECTION

// Message types
#define ERROR           255
#define ACK             1
#define INIT            2
#define DEPLOY_ITEM     3
#define PREDICTED_BNR   4

/*----------------------------Bluetooth Resources-----------------------------*/

static U8 message_type;
static U8 message_payload;

/*--------------------------------------------------------*/


bool verify(U8 msg, U8 checksum);
void marshal(U8* buf, U8 msg_type, U8 msg_payload);
bool unmarshal(U8 *buf, int buf_len);

/* LEJOS OSEK hooks */
void ecrobot_device_initialize()
{
#ifndef RUNTIME_CONNECTION
    ecrobot_init_bt_slave("1234");
#endif
}

void ecrobot_device_terminate()
{
    ecrobot_term_bt_connection();
}

/* LEJOS OSEK hook to be invoked from an ISR in category 2 */
void user_1ms_isr_type2(void)
{
    StatusType ercd;

    ercd = SignalCounter(SysTimerCnt); /* Increment OSEK Alarm Counter */
    if(ercd != E_OK)
    {
        ShutdownOS(ercd);
    }
}

bool verify(U8 msg, U8 checksum) 
{
	return msg + checksum == 0 && msg != 0 && checksum != 0;
}

bool unmarshal(U8 *buf, int buf_len) 
{
	GetResource(BluetoothResource);

	message_type = buf[0];	
	if (!verify(message_type, buf[2])) 
		return false; 

	if (buf[1] != 0) {
		message_payload = buf[1];
		if (!verify(message_payload, buf[3]))
			return false; 
	}

	ReleaseResource(BluetoothResource);
	return true;
}

// Beware of the case that the messages being sent could be 0
void marshal(U8* buf, U8 msg_type, U8 msg_payload) 
{
	GetResource(BluetoothResource);
	
	buf[0] = msg_type;
	buf[2] = ~msg_type;

	if (msg_payload != 0) {
		buf[1] = msg_payload;
		buf[3] = ~msg_payload;
	}

	ReleaseResource(BluetoothResource);
}

TASK(IdleTask) {
    static SINT bt_status = BT_NO_INIT;
	static U8 bt_send_buf[4];
	static U8 bt_recv_buf[4];

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

			bool ok = unmarshal(bt_recv_buf, read);
			if (!ok) {
				marshal(bt_send_buf, ERROR, 0);
				ecrobot_send_bt(bt_send_buf, 0, 4);
			} 

			GetResource(BluetoothResource);

			switch (message_type) {
				case ERROR:
					// TODO: we need to repeat the last sent message
					break;

				case ACK:
					// TODO: we can go to the next step, no need to answer to an ack
					ecrobot_status_monitor("Received Acknowledgement byte!!");
					break;

				case DEPLOY_ITEM:
					// TODO: we need to deploy the item from the dispenser
					break;

				case PREDICTED_BNR:
					// TODO: sort in the item into the respective bucketnumber
					break;

				default: 
					// TODO: default case is weird af
					break;

			}

			ReleaseResource(BluetoothResource);

			ecrobot_send_bt(bt_send_buf, 0, 4);
		}

        bt_status = ecrobot_get_bt_status();
	}

    TerminateTask();
}

/* IdleTask */
/* TASK(IdleTask) */
/* { */
/*     static SINT bt_status = BT_NO_INIT; */
/*     static U8 bt_send_buf[4]; */ 
/*     static U8 bt_receive_buf[4]; */

/*     while(1) */
/*     { */  
/* #ifdef RUNTIME_CONNECTION */
/*         ecrobot_init_bt_slave("1234"); */
/* #endif */

/*         if (ecrobot_get_bt_status() == BT_STREAM && bt_status != BT_STREAM) */
/*         { */
/*             bt_send_buf[0] = 13; */
/*             bt_send_buf[1] = 27; */ 

/*             ecrobot_send_bt(bt_send_buf, 0, 4); */

/*             int read = 0; */
/*             while(read == 0){ */
/*                  read = ecrobot_read_bt(bt_receive_buf, 0, 4); */
/*                  systick_wait_ms(500); */
/*             } */

/*         } */
/*         bt_status = ecrobot_get_bt_status(); */
/*     } */	
/* } */


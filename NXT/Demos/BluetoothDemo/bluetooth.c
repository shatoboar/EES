/* /1* btslave.c *1/ */ 
#include <string.h> 
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

/* OSEK declarations */
DeclareCounter(SysTimerCnt);
DeclareTask(EventDispatcher); 
DeclareTask(EventHandler);
DeclareTask(IdleTask);
DeclareEvent(TouchSensorOnEvent);
DeclareEvent(TouchSensorOffEvent); 

DeclareResource(BluetoothResource);

/* below macro enables run-time Bluetooth connection */
#define RUNTIME_CONNECTION

// Message types
#DEFINE ERROR           255
#DEFINE ACK             1
#DEFINE INIT            2
#DEFINE DEPLOY_ITEM     3
#DEFINE PREDICTED_BNR   4

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

/* EventDispatcher executed every 5ms */
TASK(EventDispatcher)
{
    static U8 bt_receive_buf[1]; 
    static U8 TouchSensorStatus_old = 0;
    U8 TouchSensorStatus; 

    /* read packet data from the master device */  
    ecrobot_read_bt_packet(bt_receive_buf, 32);
    if (bt_receive_buf[0] == 1)
    {
        ecrobot_set_motor_speed(NXT_PORT_B, 100);
    }
    else
    {
        ecrobot_set_motor_speed(NXT_PORT_B, 0);
    }  	

    TouchSensorStatus = ecrobot_get_touch_sensor(NXT_PORT_S4);
    if (TouchSensorStatus == 1 && TouchSensorStatus_old == 0)
    {
        /* Send a Touch Sensor ON Event to the Handler */ 
        SetEvent(EventHandler, TouchSensorOnEvent);
    }
    else if (TouchSensorStatus == 0 && TouchSensorStatus_old == 1)
    {
        /* Send a Touch Sensor OFF Event to the Handler */ 
        SetEvent(EventHandler, TouchSensorOffEvent);
    }
    TouchSensorStatus_old = TouchSensorStatus;

    TerminateTask();
}

/* EventHandler executed by OSEK Events */
TASK(EventHandler)
{
    static U8 bt_send_buf[32]; 

    while(1)
    {
        WaitEvent(TouchSensorOnEvent); /* Task is in waiting status until the Event comes */ 
        ClearEvent(TouchSensorOnEvent);
        /* send packet data to the master device */
        bt_send_buf[0] = 1;
        ecrobot_send_bt_packet(bt_send_buf, 32);

        WaitEvent(TouchSensorOffEvent); /* Task is in waiting status until the Event comes */
        ClearEvent(TouchSensorOffEvent);
        /* send packet data to the master device */
        bt_send_buf[0] = 0;
        ecrobot_send_bt_packet(bt_send_buf, 32);
    }

    TerminateTask();
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

	message_payload = buf[1];
	if (!verify(message_payload, buf[3]))
		return false; 
	
	ReleaseResource(BluetoothResource);
}

// Beware of the case that the messages being sent could be 0
void marshal(U8* buf, U8 msg_type, U8 msg_payload) 
{
	GetResource(BluetoothResource);
	
	buf[0] = msg_type;
	buf[2] = ~msg_type;

	if msg_payload != 0 {
		buf[1] = msg_payload;
		buf[3] = ~msg_payload;
	}

	ReleaseResource(BluetoothResource);
}

TASK(PollTask) {
	static U8 bt_send_buf[4];
	static U8 bt_recv_buf[4];

    while(1)
    {
#ifdef RUNTIME_CONNECTION
        ecrobot_init_bt_slave("1234");
#endif

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

		case ACK:
		// TODO: we can go to the next step, no need to answer to an ack

		case DEPLOY_ITEM:
		// TODO: we need to deploy the item from the dispenser

		case PREDICTED_BNR:
		// TODO: sort in the item into the respective bucketnumber

		case default: 
		// TODO: default case is weird af

		}

		ReleaseResource(BluetoothResource);

		ecrobot_send_bt(bt_send_buf, 0, 4);
    }

    TerminateTask();
}

/* IdleTask */
TASK(IdleTask)
{
    static SINT bt_status = BT_NO_INIT;
    static U8 bt_send_buf[4]; 
    static U8 bt_receive_buf[4];

    while(1)
    {  
#ifdef RUNTIME_CONNECTION
        ecrobot_init_bt_slave("1234");
#endif

        if (ecrobot_get_bt_status() == BT_STREAM && bt_status != BT_STREAM)
        {
            bt_send_buf[0] = 13;
            bt_send_buf[1] = 27; 

            ecrobot_send_bt(bt_send_buf, 0, 4);

            int read = 0;
            while(read == 0){
                 read = ecrobot_read_bt(bt_receive_buf, 0, 4);
                 systick_wait_ms(500);
            }

        }
        bt_status = ecrobot_get_bt_status();
    }	
}


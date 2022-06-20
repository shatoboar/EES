/* eds.c */
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"
#include <stdbool.h>

/* OSEK declarations */
DeclareCounter(SysTimerCnt);

DeclareTask(EventDispatcher);
DeclareTask(EventCallback);
DeclareTask(EventRightCallback);
DeclareTask(TaskLCD);

DeclareEvent(TouchSensorOnEvent); /* Event declaration */
DeclareEvent(TouchSensorOffEvent); /* Event declaration */
DeclareEvent(RightSensorOnEvent);
DeclareEvent(RightSensorOffEvent);

DeclareResource(ResourceNumber);


//global variables:
static U32 val = 0;
static U32 count = 0;

/* nxtOSEK hooks */
void ecrobot_device_initialize(void)
{
    nxt_motor_set_speed(NXT_PORT_A, 0, 1);
    nxt_motor_set_speed(NXT_PORT_B, 0, 1);

}

void ecrobot_device_terminate(void)
{
    nxt_motor_set_speed(NXT_PORT_A, 0, 1);
    nxt_motor_set_speed(NXT_PORT_B, 0, 1);

}

/* nxtOSEK hook to be invoked from an ISR in category 2 */
void user_1ms_isr_type2(void)
{
    StatusType ercd;

    ercd = SignalCounter(SysTimerCnt); /* Increment OSEK Alarm Counter */
    if (ercd != E_OK)
    {
        ShutdownOS(ercd);
    }
}

/* EventDispatcher executed every 1ms */
TASK(EventDispatcher)
{
    U8 isPressed = 0;
    U8 isPressedRight = 0;

    static bool wasPressedLastTick = false;
    static bool wasPressedRightLastTick = false;

    //middle big button dispatching
    isPressed = ecrobot_is_ENTER_button_pressed();
    if (isPressed == 1 && !wasPressedLastTick) {
        SetEvent(EventCallback, TouchSensorOnEvent);
    } else if (isPressed == 0 && wasPressedLastTick) {
        SetEvent(EventCallback, TouchSensorOffEvent);
    }
    wasPressedLastTick = isPressed;


    //right button dispatching
    isPressedRight = ecrobot_is_RUN_button_pressed();
    if (isPressedRight == 1 && !wasPressedRightLastTick) {
        SetEvent(EventRightCallback, RightSensorOnEvent);
    } else if (isPressedRight == 0 && wasPressedRightLastTick) {
        SetEvent(EventRightCallback, RightSensorOffEvent);
    }
    wasPressedRightLastTick = isPressedRight;    


    TerminateTask();
}

/* EventCallback executed by OSEK Events */
TASK(EventCallback)
{

    while(1)
    {
        WaitEvent(TouchSensorOnEvent); /* Task is in waiting status until the Event comes */
        ClearEvent(TouchSensorOnEvent);
        nxt_motor_set_speed(NXT_PORT_C, 50, 1);

        GetResource(ResourceNumber);
        val = 100;
        ReleaseResource(ResourceNumber);

        WaitEvent(TouchSensorOffEvent);
        ClearEvent(TouchSensorOffEvent);
        nxt_motor_set_speed(NXT_PORT_C, 0, 1);

        GetResource(ResourceNumber);
        val = 0;
        ReleaseResource(ResourceNumber);

    }

    TerminateTask();
}
/*
   TASK(MainTask)
   {
   while (1) 
   {

   WaitEvent(TouchSensorOnEvent); /* Task is in waiting status until the Event comes *//*
  ClearEvent(TouchSensorOnEvent);
  dispense();
  moveToCamera();
  int numberBox = getCameraData();

  currentPosition = moveToBox(numberBox, currentPosition);
  dropItem();

  }
  }

  void dispense() {
  nxt_motor_set_speed(NXT_PORT_A, 10, 1);
  }
*/



TASK(EventRightCallback) 
{
    while (1) 
    {
        WaitEvent(RightSensorOnEvent);
        ClearEvent(RightSensorOnEvent);
        //move only 180 degree
        nxt_motor_set_speed(NXT_PORT_A, 50, 1);
        U32 motor_count = 0;
        motor_count = nxt_motor_get_count(NXT_PORT_A);
        while (motor_count < 180) {
            motor_count = nxt_motor_get_count(NXT_PORT_A);
        }
        nxt_motor_set_speed(NXT_PORT_A, 0, 1);
        count = nxt_motor_get_count(NXT_PORT_A);
        nxt_motor_set_count(NXT_PORT_A, 0);

    TASK(EventRightCallback) 
    {
      while (1) 
      {
        WaitEvent(RightSensorOnEvent);
        ClearEvent(RightSensorOnEvent);
        
        nxt_motor_set_speed(NXT_PORT_C, -50, 1);

        WaitEvent(RightSensorOffEvent);
        ClearEvent(RightSensorOffEvent);

        nxt_motor_set_speed(NXT_PORT_C, 0, 1);

    }

    TerminateTask();
}


/* TaskLCD executed every 100ms */
TASK(TaskLCD)
{
    display_clear(1);

    GetResource(ResourceNumber);
    U32 copy_val = val;        
    ReleaseResource(ResourceNumber);

    display_goto_xy(0, 0);
    display_string("Motor Speed:");
    display_goto_xy(0, 1);
    display_unsigned(copy_val, 2);
    display_goto_xy(0, 2);
    display_string("Motor Count:");
    display_goto_xy(0, 3);
    U32 motor_count = nxt_motor_get_count(NXT_PORT_A); //gets motor_count in degree
    display_unsigned(motor_count, 4);
    display_goto_xy(0, 4);
    display_unsigned(count, 4);
    display_update();

    TerminateTask();
}

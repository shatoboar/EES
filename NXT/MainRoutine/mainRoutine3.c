

/* eds.c */
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"
#include <stdbool.h>

#include "duties.h"
#include "motor_helper.h"

#include "mainRoutine3.h"
#include "mainTask.h"




/* nxtOSEK hooks */
void ecrobot_device_initialize(void)
{
  nxt_motor_set_speed(NXT_PORT_A, 0, 1);
  nxt_motor_set_speed(NXT_PORT_B, 0, 1);
  ecrobot_set_light_sensor_active(NXT_PORT_S1);


}

void ecrobot_device_terminate(void)
{
  nxt_motor_set_speed(NXT_PORT_A, 0, 1);
  nxt_motor_set_speed(NXT_PORT_B, 0, 1);
  ecrobot_set_light_sensor_inactive(NXT_PORT_S1);
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
TASK(EventDispatcherTask)
{
    U8 isPressed = 0;
    U8 isPressedRight = 0;

    static bool wasPressedLastTick = false;
    static bool wasPressedRightLastTick = false;

    static bool isOnStartup = true;


    
    //middle big button dispatching
    isPressed = ecrobot_is_ENTER_button_pressed();
    if (isPressed == 1 && !wasPressedLastTick) {

      //calibrate Threshold
      //Poll light data
      U16 light_data = ecrobot_get_light_sensor(NXT_PORT_S1);
      light_data += 50;
      GetResource(ResourceLightThreshold);
      LIGHT_THRESHOLD = light_data;
      ReleaseResource(ResourceLightThreshold);

    } else if (isPressed == 0 && wasPressedLastTick) {
      SetEvent(EventBigButtonTask, BigButtonOffEvent);
    }
    wasPressedLastTick = isPressed;

    /*
    //right button dispatching
    isPressedRight = ecrobot_is_RUN_button_pressed();
    if (isPressedRight == 1 && !wasPressedRightLastTick) {
        SetEvent(EventRightButtonTask, RightButtonOnEvent);
    } else if (isPressedRight == 0 && wasPressedRightLastTick) {
        SetEvent(EventRightButtonTask, RightButtonOffEvent);
    }
    wasPressedRightLastTick = isPressedRight;    


    
    U16 light_data = ecrobot_get_light_sensor(NXT_PORT_S1);
    GetResource(ResourceNumber);
    val = light_data;
    ReleaseResource(ResourceNumber);
    if (val >= 500) {
        nxt_motor_set_speed(NXT_PORT_B, 100, 1);
    } else {
        //nxt_motor_set_speed(NXT_PORT_B, 0, 1);
        //moveOneUnit(1, 100);
        SetEvent(ThrowStoneTask, ThrowStoneEvent);
    }*/

    TerminateTask();
}


TASK(EventBigButtonTask)
{
  while(1)
  {
      WaitEvent(BigButtonOnEvent);
      ClearEvent(BigButtonOnEvent);


      WaitEvent(BigButtonOffEvent);
      ClearEvent(BigButtonOffEvent);
  }

  TerminateTask();
}

TASK(EventRightButtonTask) 
{
  while (1) 
  {
      
      WaitEvent(RightButtonOnEvent);
      ClearEvent(RightButtonOnEvent);

      WaitEvent(RightButtonOffEvent);
      ClearEvent(RightButtonOffEvent);
  }

  TerminateTask();
}




/* LCDTask executed every 100ms */
TASK(LCDTask)
{
    display_clear(1);
    display_goto_xy(0, 0);
    display_string("Test");

    display_goto_xy(0, 1);
    display_string("LightThres:");
    display_unsigned((U32) LIGHT_THRESHOLD, 4);

    display_goto_xy(0, 2);
    display_string("Boxesleft:");
    display_unsigned((U32) BOXES_REMAINING, 1);

    display_goto_xy(0, 3);
    display_string("Direction:");
    display_unsigned((U32) DIRECTION, 1);
    display_update();

    display_goto_xy(0, 4);
    display_string("Mode:");
    display_unsigned((U32) MODE, 1);
    display_update();
  

    TerminateTask();
}
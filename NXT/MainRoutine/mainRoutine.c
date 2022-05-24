

    /* eds.c */
    #include "kernel.h"
    #include "kernel_id.h"
    #include "ecrobot_interface.h"
    #include <stdbool.h>

    #include "duties.h"
    #include "motor_helper.h"

    /* OSEK declarations */
    DeclareCounter(SysTimerCnt);

    DeclareTask(EventDispatcherTask);
    DeclareTask(EventBigButtonTask);
    DeclareTask(EventRightButtonTask);
    DeclareTask(LCDTask);
    DeclareTask(ThrowStoneTask);

    DeclareEvent(BigButtonOnEvent); /* Event declaration */
    DeclareEvent(BigButtonOffEvent); /* Event declaration */
    DeclareEvent(RightButtonOnEvent);
    DeclareEvent(RightButtonOffEvent);
    DeclareEvent(ThrowStoneOnEvent);

    DeclareResource(ResourceNumber);


    //global variables:
    static U16 val = 0;
    static U32 count = 0;

    //position in degrees of the motor
    static int currentPrecisePosition;
    //number of boxes
    static int numberBoxes;
    /*
     * Contains all precise positions of the boxes.
     * A precise position is measured in the beginning in degrees.
     * 
     * Example:
     * [0, 500, 607, 1000]
     *            - from left point 
     */
    static int boxesPrecisePositions[] = {0, 500, 600, 1000};



    /* nxtOSEK hooks */
    void ecrobot_device_initialize(void)
    {
      nxt_motor_set_speed(NXT_PORT_A, 0, 1);
      nxt_motor_set_speed(NXT_PORT_B, 0, 1);
      ecrobot_set_light_sensor_active(NXT_PORT_S1);

      //simulate calibration:
      currentPrecisePosition = 0;
      numberBoxes = 4;
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
/*
        //middle big button dispatching
        isPressed = ecrobot_is_ENTER_button_pressed();
        if (isPressed == 1 && !wasPressedLastTick) {
            SetEvent(EventBigButtonTask, BigButtonOnEvent);
        } else if (isPressed == 0 && wasPressedLastTick) {
          SetEvent(EventBigButtonTask, BigButtonOffEvent);
        }
        wasPressedLastTick = isPressed;


      //right button dispatching
      isPressedRight = ecrobot_is_RUN_button_pressed();
      if (isPressedRight == 1 && !wasPressedRightLastTick) {
          SetEvent(EventRightButtonTask, RightButtonOnEvent);
      } else if (isPressedRight == 0 && wasPressedRightLastTick) {
        SetEvent(EventRightButtonTask, RightButtonOffEvent);
      }
      wasPressedRightLastTick = isPressedRight;    
*/


      U16 light_data = ecrobot_get_light_sensor(NXT_PORT_S1);
      GetResource(ResourceNumber);
      val = light_data;
      ReleaseResource(ResourceNumber);
      if (val >= 500) {
        nxt_motor_set_speed(NXT_PORT_B, 100, 1);
      } else {
        //nxt_motor_set_speed(NXT_PORT_B, 0, 1);
        //moveOneUnit(1, 100);
        SetEvent(ThrowStoneTask, ThrowStoneOnEvent);
      }

      TerminateTask();
    }

    TASK(ThrowStoneTask) {
      while (1)
      {
        WaitEvent(ThrowStoneOnEvent);
        ClearEvent(ThrowStoneOnEvent);
        nxt_motor_set_speed(NXT_PORT_B, 0, 1);
        dropItem();
      }

      TerminateTask();
    }

    /* EventCallback executed by OSEK Events */
    TASK(EventBigButtonTask)
    {
      
      while(1)
      {
        WaitEvent(BigButtonOnEvent); /* Task is in waiting status until the Event comes */
        ClearEvent(BigButtonOnEvent);
       // nxt_motor_set_count(NXT_PORT_A, 100000);
        //nxt_motor_set_speed(NXT_PORT_A, -100, 1);
        //nxt_motor_set_speed(NXT_PORT_B, 100, 1);

      //movesDegrees(NXT_PORT_A, 180, 100);

        /*

        moveOneUnit(1, 100);
        moveOneUnit(-1, 100);
        moveOneUnit(-1, 100);
        moveOneUnit(-1, 100);
        moveOneUnit(1, 100);

        moveOneUnit(-1, 100);
        moveOneUnit(1, 100);
*/
        



        WaitEvent(BigButtonOffEvent);
        ClearEvent(BigButtonOffEvent);
        //nxt_motor_set_speed(NXT_PORT_A, 0, 1);
        //nxt_motor_set_speed(NXT_PORT_B, 0, 1);

        GetResource(ResourceNumber);
        //val = 0;
        ReleaseResource(ResourceNumber);

      }

      TerminateTask();
    }

    TASK(EventRightButtonTask) 
    {
      while (1) 
      {
        WaitEvent(RightButtonOnEvent);
        ClearEvent(RightButtonOnEvent);
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

        WaitEvent(RightButtonOffEvent);
        ClearEvent(RightButtonOffEvent);

        nxt_motor_set_speed(NXT_PORT_A, 0, 1);

      }

      TerminateTask();
    }


    /* LCDTask executed every 100ms */
    TASK(LCDTask)
    {
      display_clear(1);

      GetResource(ResourceNumber);
      U16 copy_val = val;        
      ReleaseResource(ResourceNumber);

      display_goto_xy(0, 0);
      display_string("Light data:");
      display_goto_xy(0, 1);
      display_unsigned(copy_val, 7);
      display_goto_xy(0, 2);
      display_string("Motor Count:");
      display_goto_xy(0, 3);
      int motor_count = nxt_motor_get_count(NXT_PORT_A); //gets motor_count in degree
      display_int(motor_count, 6);
      display_goto_xy(0, 4);
      display_unsigned(count, 4);
      display_update();

      TerminateTask();
    }

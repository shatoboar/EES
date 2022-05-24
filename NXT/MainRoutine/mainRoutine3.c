

    /* eds.c */
    #include "kernel.h"
    #include "kernel_id.h"
    #include "ecrobot_interface.h"
    #include <stdbool.h>

    #include "duties.h"
    #include "motor_helper.h"

    #define THROW_STONE_TASK         0
    #define MOVE_TO_BOX_TASK         1
    #define TMP_MOVE_TASK            2
    #define READY_FOR_STONE_TASK     3
    #define IDLE_TASK                4

    /* OSEK declarations */
    DeclareCounter(SysTimerCnt);

    DeclareTask(EventDispatcherTask);
    DeclareTask(EventBigButtonTask);
    DeclareTask(EventRightButtonTask);
    DeclareTask(LCDTask);
    DeclareTask(MainTask);

    DeclareEvent(BigButtonOnEvent); /* Event declaration */
    DeclareEvent(BigButtonOffEvent); /* Event declaration */
    DeclareEvent(RightButtonOnEvent);
    DeclareEvent(RightButtonOffEvent);

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

        static bool isOnStartup = true;


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

    TASK(MainTask) {
      while (1)
      {

        /**
         *  0: ThrowStoneTask
         *  1: MoveToBoxTask
         *  2: TmpMoveTask
         *  3: ReadyForStoneTask
         *  4: IDLE_TASK
         */

        static int MODE = READY_FOR_STONE_TASK;
        /**
         * 1 = forward direction
         * -1 = backward direction
         */
        static int direction = 1;

        /**
         * Number of boxes, which are remaining until the correct box is found.
         */
        static int boxesRemaining = 3;

        /**
         * Should be set to True if MOVE_TO_BOX_TASK or TMP_MOVE_TASK is entered.
         * Sets the speed again.
         */
        static bool shouldSetSpeed = true;

        
        switch(MODE) {
          case THROW_STONE_TASK:
            movesDegrees(NXT_PORT_A, 360, 100);
            
            MODE = IDLE_TASK;
          break;


          case MOVE_TO_BOX_TASK:
            //Move robot until a calibration stone is found
            if (shouldSetSpeed) {
              nxt_motor_set_speed(NXT_PORT_B, 100 * direction, 1);
              shouldSetSpeed = false;
            }

            //Poll light data
            U16 light_data = ecrobot_get_light_sensor(NXT_PORT_S1);
            if (light_data <= 500) {
              //stop motor and decrease boxesRemaining
              nxt_motor_set_speed(NXT_PORT_B, 0, 1);

              boxesRemaining--;

              if (boxesRemaining == 0) {
                
                MODE = THROW_STONE_TASK;
              } else { //still have to go some more boxes

                shouldSetSpeed = true;
                MODE = TMP_MOVE_TASK;  
              }
            
            } 
          break;


          case TMP_MOVE_TASK:
            if (shouldSetSpeed) {     
              movesDegrees(NXT_PORT_B, 180, 80);
              //movesDegrees(NXT_PORT_B, 350, 30);
              //movesDegrees(NXT_PORT_B, 200, -30);
              shouldSetSpeed = false;
            }

            shouldSetSpeed = true;
            MODE = MOVE_TO_BOX_TASK;
          break;
          

          case READY_FOR_STONE_TASK:

            MODE = MOVE_TO_BOX_TASK;
          break;

          case IDLE_TASK:
            nxt_motor_set_speed(NXT_PORT_A, 0, 1);
            nxt_motor_set_speed(NXT_PORT_B, 0, 1);
          break;
        }
      }

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

        display_update();

        TerminateTask();
    }

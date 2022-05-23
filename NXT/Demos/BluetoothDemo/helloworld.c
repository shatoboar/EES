/* helloworld.c for TOPPERS/ATK(OSEK) */ 
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

/* nxtOSEK hook to be invoked from an ISR in category 2 */
void user_1ms_isr_type2(void){ /* do nothing */ }

// PINCODE: "MATLAB"
// If the connection established here is unstable, we can try to use a background Task
void ecrobot_device_initialize() 
{
    // This function returns 0 if unsuccessful
    ecrobot_set_bt_device_name("nxtOsek");
    
    int status = ecrobot_get_bt_status();

    while (status != BT_STREAM) 
    {
        ecrobot_init_bt_connection();
        status = ecrobot_get_bt_status();
		systick_wait_ms(500); /* 500msec wait */
    }
}


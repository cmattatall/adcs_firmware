#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if defined(TARGET_MCU)
#include "watchdog.h"
#include "mcu.h"
#include "timer_a.h"
#include "magnetorquers.h"
#include "magnetometer.h"
#else
#include <errno.h>
#endif /* #if defined(TARGET_MCU) */

#include "obc_interface.h"
#include "jsons.h"

/* NOTE THESE 2 HEADERS ARE JUST TEMPORARY STUFF  */

static uint8_t msg[128];

int main(void)
{

#if defined(TARGET_MCU)
#if defined(DEBUG)
    watchdog_stop();
#else
    watchdog_start();
#endif /* #if defined(DEBUG) */

    OBC_IF_config(OBC_IF_PHY_CFG_UART);

    MQTR_init();
    enable_interrupts();

#else
    OBC_IF_config(OBC_IF_PHY_CFG_EMULATED);
#endif /* #if defined(TARGET_MCU) */
    

    volatile int i = 0;
    while (1)
    {   

        i = 0;
        while(++i < 10000);


        OBC_IF_printf("Hello World");
        
        #if 0
        if (OBC_IF_dataRxFlag_read() == OBC_IF_DATA_RX_FLAG_SET)
        {
            /* get command json string from OBC interface */
            OCB_IF_get_command_string(msg, sizeof(msg));

            /* Parse command json string */
            if (0 != json_parse(msg))
            {
                OBC_IF_printf(
                    "{\"error\" : \"json format\", \"received\":\"%s\"}\r\n",
                    msg);
            }
            OBC_IF_dataRxFlag_write(OBC_IF_DATA_RX_FLAG_CLR);
        }

        #endif

#if defined(TARGET_MCU) && !defined(DEBUG)
        watchdog_kick();
#endif /* #if defined(TARGET_MCU) && !defined(DEBUG)*/
    }
}

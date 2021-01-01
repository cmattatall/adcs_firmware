#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if defined(TARGET_MCU)
#include "watchdog.h"
#include "mcu.h"
#include "bsp.h"
#include "timer_a0.h"
#else
#endif /* #if defined(TARGET_MCU) */

#include "obc_interface.h"
#include "jsons.h"

#include "injection_api.h"

/* NOTE THESE 2 HEADERS ARE JUST TEMPORARY STUFF  */

static uint8_t msg[128];


int main(void)
{
#if defined(TARGET_MCU) && defined(DEBUG)
    watchdog_stop();
#elif defined(TARGET_MCU) && !defined(DEBUG)
    watchdog_start();
#endif


#if defined(TARGET_MCU)
    OBC_IF_config(OBC_IF_PHY_CFG_UART);

    BSP_init();

    TIMERA0_heartbeat_init();

    callback_handle_t heartbeat_cb = new_callback(BSP_toggle_red_led, NULL);
    TIMERA0_register_callback(heartbeat_cb);

    /* This should be the very last thing that occurs */
    enable_interrupts();
#else
    OBC_IF_config(OBC_IF_PHY_CFG_EMULATED);
#endif /* #if defined(TARGET_MCU) */

    while (1)
    {
        if (OBC_IF_dataRxFlag_read() == OBC_IF_DATA_RX_FLAG_SET)
        {
            /* get command json string from OBC interface */
            OCB_IF_get_command_string(msg, sizeof(msg));

            /* Parse command json string */
            if (0 != json_parse(msg))
            {
                OBC_IF_printf("{\"error\" : \"json format\"}\n");
            }
            OBC_IF_dataRxFlag_write(OBC_IF_DATA_RX_FLAG_CLR);
        }

#if defined(TARGET_MCU) && !defined(DEBUG)
        watchdog_kick();
#endif /* #if defined(TARGET_MCU) && !defined(DEBUG)*/
    }
}

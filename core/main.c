#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if defined(TARGET_MCU)
#include "spi.h"
#include "uart.h"
#include "watchdog.h"
#include "mcu.h"
#include "bsp.h"
#include "timer_a0.h"
#else
#include "obc_emulator.h"
#endif /* #if defined(TARGET_MCU) */

#include "obc_interface.h"
#include "jsons.h"

#include "injection_api.h"

/* NOTE THESE 2 HEADERS ARE JUST TEMPORARY STUFF  */

static uint8_t msg[500];


int main(void)
{
#if defined(TARGET_MCU)
    watchdog_stop();

    OBC_IF_config(uart_init, uart_deinit, uart_transmit);

    BSP_init();

    TIMERA0_heartbeat_init();

    callback_handle_t heartbeat_cb = new_callback(BSP_toggle_red_led, NULL);
    TIMERA0_register_callback(heartbeat_cb);

    /* This should be the very last thing that occurs */
    enable_interrupts();
#else
    OBC_IF_config(NULL, NULL, OBC_EMU_tx);
    OBC_EMU_start();
#endif /* #if defined(TARGET_MCU) */

    while (1)
    {
        if (OBC_IF_dataRxFlag_read() == OBC_IF_DATA_RX_FLAG_SET)
        {

            /* get command json string from OBC interface */
            OCB_IF_get_command_string(msg, sizeof(msg));
            /* Parse command json string */

            if (0 != json_parse(msg, strnlen((char *)msg, sizeof(msg))))
            {
                uint8_t error_message[] = "{\"error\" : \"json format\"}\n";
                OBC_IF_printf("%s\n", error_message);
            }

            OBC_IF_dataRxFlag_write(OBC_IF_DATA_RX_FLAG_CLR);
        }
    }
}

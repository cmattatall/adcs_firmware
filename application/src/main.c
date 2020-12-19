#include <stdint.h>

#if defined(TARGET_MCU)
#include "spi.h"
#include "uart.h"
#include "watchdog.h"
#include "mcu.h"
#else
#include "obc_emulator.h"
#endif /* #if defined(TARGET_MCU) */

#include "obc_interface.h"
#include "jsons.h"

/* NOTE THESE 2 HEADERS ARE JUST TEMPORARY STUFF  */
#include <msp430.h>
#include "timers.h"

static uint8_t json_buffer[500];


void BSP_configure_red_led(void)
{
    P1DIR = 0xFF; // P1.0 output
}

int main(void)
{
#if defined(TARGET_MCU)
    watchdog_stop();

    // SPI0_init(&SPI0_RX_signal_watcher, &SPI0_TX_signal_watcher);
    // OBC_IF_config(uart_init, uart_deinit, uart_transmit);

    BSP_configure_red_led();
    TIMERA0_init();

    enable_interrupts(); /* This should be the very last thing that occurs */

#else
    OBC_IF_config(NULL, NULL, OBC_EMU_tx);
    OBC_EMU_start();
#endif /* #if defined(TARGET_MCU) */

    while (1)
    {
#if 0

        if (OBC_IF_dataRxFlag_read() == OBC_IF_DATA_RX_FLAG_SET)
        {

            /* get command json string from OBC interface */
            OCB_IF_get_command_string(json_buffer, sizeof(json_buffer));

            /* Parse command json string */
            int parse_status = json_parse(json_buffer, sizeof(json_buffer));
            if (JSON_PARSE_ERROR(parse_status))
            {
                uint8_t error_message[] = "{\"error\" : \"json format\"}\n";
                OBC_IF_tx(error_message, sizeof(error_message));
            }
            else if (JSON_PARSE_UNK(parse_status))
            {
                uint8_t message[] = "{\"ADCS\" : \"unknown command\"}\n";
                OBC_IF_tx(message, sizeof(message));
            }
            else
            {
                /* successful, don't do anything */
            }

            OBC_IF_dataRxFlag_write(OBC_IF_DATA_RX_FLAG_CLR);
        }
#endif
        /*
        if (heartbeat_flag)
        {
            
            heartbeat_flag = false;
        }
        */
    }
}

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

#define TESTMSG (uint8_t *)"Hello World\r\n"

static uint8_t json_buffer[500];

static void periph_init(void);

#if defined(TARGET_MCU)

static uint8_t       user_spi_rx_buf[200];
static volatile int *SPI0_RX_signal_watcher;
static volatile int *SPI0_TX_signal_watcher;

#endif /* #if defined(TARGET_MCU) */


int main(void)
{
    periph_init(); /* peripheral initialization */

    while (1)
    {

#if defined(TARGET_MCU)

        /** @todo I've put the SPI stuff inside an IFDEF block for now.
         *        but in the future, need to wrap the SPI stuff in an interface
         *        API for the actual hardware it is controlling.
         *        - Carl
         */

        if (*SPI0_TX_signal_watcher == SPI_SIGNAL_SET) /* Transmit if we can */
        {
            // unsigned int bytes_loaded =
            SPI0_transmit_IT(TESTMSG, sizeof(TESTMSG));
        }

        if (*SPI0_RX_signal_watcher == SPI_SIGNAL_SET)
        {
            SPI0_receive_payload(user_spi_rx_buf, sizeof(user_spi_rx_buf));

            /** @todo DO STUFF WITH THE SPI DATA */
        }

#endif /* #if defined(TARGET_MCU) */

        if (OBC_IF_dataRxFlag_read() == OBC_IF_DATA_RX_FLAG_SET)
        {
            /* get command json string from OBC interface */
            OCB_IF_get_command_string(json_buffer, sizeof(json_buffer));

            /* Parse command json string */
            int parse_status = json_parse(json_buffer, sizeof(json_buffer));
            if (JSON_PARSE_ERROR(parse_status))
            {
                uint8_t error_message[] = "{\"error\" : \"json format\"}";
                OBC_IF_tx(error_message, sizeof(error_message));
            }
            else if (JSON_PARSE_UNK(parse_status))
            {
                uint8_t message[] = "{\"ADCS\" : \"unknown command\"}";
                OBC_IF_tx(message, sizeof(message));
            }
            else
            {
                /* successful, don't do anything */
            }

            OBC_IF_dataRxFlag_write(OBC_IF_DATA_RX_FLAG_CLR);
        }
    }
}


static void periph_init(void)
{
#if defined(TARGET_MCU)
#if defined(DEBUG)
    watchdog_stop();
#else
    watchdog_start();
#endif /* #if defined(DEBUG) */

    SPI0_init(&SPI0_RX_signal_watcher, &SPI0_TX_signal_watcher);
    OBC_IF_config(uart_init, uart_deinit, uart_transmit);

    enable_interrupts(); /* This should be the very last thing that occurs */
#else
    OBC_EMU_start();
#endif /* #if defined(TARGET_MCU) */
}

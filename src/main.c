#include <msp430.h>
#include <stdint.h>

#include "spi.h"
#include "obc_interface.h"
#include "uart.h"

#include "jsons.h"

#define TESTMSG "Hello World\r\n"

static uint8_t       user_spi_rx_buf[200];
static volatile int *SPI0_RX_signal_watcher;
static volatile int *SPI0_TX_signal_watcher;


static uint8_t json_buffer[500];

void main(void)
{

    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
    SPI0_init(&SPI0_RX_signal_watcher, &SPI0_TX_signal_watcher);
    OBC_IF_config(uart_init, uart_deinit, uart_transmit);

    //__bis_SR_register(LPM0_bits + GIE); // Enter LPM0, interrupts enabled
    __bis_SR_register(GIE); // Enable interrupts globally
    while (1)
    {
        /* Transmit if we can */
        if (*SPI0_TX_signal_watcher == SPI_SIGNAL_SET)
        {
            unsigned int bytes_loaded =
                SPI0_transmit_IT(TESTMSG, sizeof(TESTMSG));
        }

        if (*SPI0_RX_signal_watcher == SPI_SIGNAL_SET)
        {
            SPI0_receive_payload(user_spi_rx_buf, sizeof(user_spi_rx_buf));
        }

        if (OBC_IF_data_received_flag)
        {
            OCB_IF_get_command_string(json_buffer, sizeof(json_buffer));
            int parse_status = json_parse(json_buffer, sizeof(json_buffer));

            /* JTOK uses negative retvals for error codes */
            if (parse_status < 0)
            {
                uint8_t error_message[] = "{\"error\" : \"json format\"}";
                OBC_IF_tx(error_message, sizeof(error_message));
            }

            /* Positive retval means parse was
             * successful but we didn't know
             * what to do with it
             *
             * (unknown or unsupported command)
             */
            else if (parse_status > 0)
            {
                uint8_t message[] = "{\"ADCS\" : \"unknown command\"}";
                OBC_IF_tx(message, sizeof(message));
            }

            OBC_IF_data_received_flag = false;
        }
    }
}

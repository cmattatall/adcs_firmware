#include <msp430.h>
#include <stdint.h>

#include "spi.h"
#include "jtok.h"
#include "obc_interface.h"
#include "uart.h"

#define TESTMSG "Hello World\r\n"

static uint8_t       user_spi_rx_buf[200];
static volatile int *SPI0_RX_signal_watcher;
static volatile int *SPI0_TX_signal_watcher;

void main(void)
{

    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
    __bis_SR_register(GIE);   // Enable interrupts globally
    SPI0_init(&SPI0_RX_signal_watcher, &SPI0_TX_signal_watcher);
    OBC_if_config(uart_init, uart_deinit, uart_receive, uart_transmit);

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

        if (OBC_if_data_received_flag)
        {
            /** @todo
             *
             *  - GET DATA
             *
             *  - PARSE DATA
             *
             *  - DO STUFF WITH THE DATA
             */
            OBC_if_data_received_flag = false;
        }
    }
}

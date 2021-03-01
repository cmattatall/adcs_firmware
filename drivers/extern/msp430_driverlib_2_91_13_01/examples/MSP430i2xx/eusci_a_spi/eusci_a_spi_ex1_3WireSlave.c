/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/*******************************************************************************
 * MSP430i2xx EUSCI_A SPI - 3 Wire Incrementing Data Slave
 *
 * Description: In this example, the device is configured as a SPI slave. The
 * initial data is written to the TX Buffer. The master will start a transfer
 * which clears the TX buffer on the slave and causes an RX interrupt that
 * contains the data it recieved from the master. The slave and master data
 * values should stay in sync and if there is a problem in communication the
 * LED on the master will turn on. The device stays in LPM0 and sends the data
 * in the ISR. To properly demo this example connect to master version of this
 * example and start the slave device first.
 *
 *                MSP430i2041
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST  P1.3/UCA0SIMO|<---Data In
 *            |                  |
 *            |     P1.2/UCA0MISO|--->Data Out
 *            |                  |
 *            |      P1.1/UCA0CLK|<--Serial Clock
 *            |                  |
 *
 * Author: Zack Lalanne
 ******************************************************************************/
#include "driverlib.h"

volatile uint8_t TXData = 0;
volatile uint8_t RXData = 0;

int main(void) {

    EUSCI_A_SPI_initSlaveParam spiSlaveConfig = {
        EUSCI_A_SPI_MSB_FIRST,                          // MSB First
        EUSCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,  // Phase
        EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_HIGH,     // Normal Polarity
        EUSCI_A_SPI_3PIN                               // 3wire mode
    };


    WDT_hold(WDT_BASE);

    // Setting P1.1, P1.2 and P1.3 as SPI pins.
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                                               GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    // Setting the DCO to use the internal resistor. DCO will be at 16.384MHz
    // ACLK is at 32kHz
    CS_setupDCO(CS_INTERNAL_RESISTOR);

    // Configure and enable the SPI peripheral
    EUSCI_A_SPI_initSlave(EUSCI_A0_BASE, &spiSlaveConfig);
    EUSCI_A_SPI_enable(EUSCI_A0_BASE);

    // Put the first byte in the transfer buffer
    EUSCI_A_SPI_transmitData(EUSCI_A0_BASE, TXData);

    EUSCI_A_SPI_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);

    // Go into LPM0 with interrupts enabled
    __bis_SR_register(LPM0_bits | GIE);
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_A0_VECTOR)))
#endif
void USCI_A0_ISR(void) {

    switch(__even_in_range(UCA0IV, USCI_SPI_UCTXIFG)) {
        case USCI_NONE: break;
        case USCI_SPI_UCRXIFG:
            // Put next byte in buffer
            EUSCI_A_SPI_transmitData(EUSCI_A0_BASE, ++TXData);
            break;
        case USCI_SPI_UCTXIFG:
            break;
        default: break;
    }
}

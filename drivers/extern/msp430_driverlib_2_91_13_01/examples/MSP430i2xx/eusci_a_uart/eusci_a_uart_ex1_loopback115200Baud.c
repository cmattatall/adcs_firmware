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
 * MSP430i2xx EUSCI_A UART - Loopback at 115200 Baud
 *
 * Description: In this example, the UART RX and TX pins are connected to
 * create a loopback of communication data. The example shows how to properly
 * initialize UART communication at baud rate of 115200 using SMCLK as the
 * source. After the byte is put into the buffer to send the device goes into
 * LPM until it has received the sent byte. If data corruption occurs then
 * the LED will be turned on indicating a problem.
 *
 *
 *                MSP430i2041
 *             ------------------
 *         /|\|                  |
 *          | |              P1.4|--->LED
 *          --|RST               |
 *            |      P1.2/UCA0RXD|----|
 *            |                  |    |
 *            |                  |    |
 *            |      P1.3/UCA0TXD|----|
 *            |                  |
 *
 * Author: Zack Lalanne
 ******************************************************************************/

#include "driverlib.h"

uint8_t TXData = 1;
uint8_t RXData = 0;

int main(void)
{

    // Configuration for 115200 UART with SMCLK at 16384000
    // These values were generated using the online tool available at:
    // http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
    EUSCI_A_UART_initParam uartConfig = {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        8,                                       // BRDIV = 8
        14,                                      // UCxBRF = 14
        34,                                      // UCxBRS = 34
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_MSB_FIRST,                  // MSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling Baudrate
    };

    WDT_hold(WDT_BASE);

    // Setting the DCO to use the internal resistor. DCO will be at 16.384MHz
    CS_setupDCO(CS_INTERNAL_RESISTOR);

    // SMCLK should be same speed as DCO. SMCLK = 16.384MHz
    CS_initClockSignal(CS_SMCLK, CS_CLOCK_DIVIDER_1);

    // Settings P1.2 and P1.3 as UART pins. P1.4 as LED output
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
            GPIO_PIN2 | GPIO_PIN3,
            GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN4);

    // Configure and enable the UART peripheral
    EUSCI_A_UART_init(EUSCI_A0_BASE, &uartConfig);
    EUSCI_A_UART_enable(EUSCI_A0_BASE);

    EUSCI_A_UART_enableInterrupt(EUSCI_A0_BASE,
            EUSCI_A_UART_RECEIVE_INTERRUPT);

    while(1)
    {
        EUSCI_A_UART_transmitData(EUSCI_A0_BASE, TXData);

        // Go to sleep and wait for LPM exit
        __bis_SR_register(LPM0_bits | GIE);
    }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_A0_VECTOR)))
#endif
void USCI_A0_ISR(void)
{
    switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG)) {
        case USCI_NONE: break;
        case USCI_UART_UCRXIFG:

                        RXData = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);

                        // Check if data got corrupted
                        if(RXData != TXData) {
                            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN4);
                            while(1);
                        }

                        TXData++;

                        // Leave LPM so next byte can be sent
                        __bic_SR_register_on_exit(LPM0_bits);

                        break;
        case USCI_UART_UCTXIFG: break;
        case USCI_UART_UCSTTIFG: break;
        case USCI_UART_UCTXCPTIFG: break;
        default: break;
    }
}


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
//******************************************************************************
//! This example shows how to configure the UART module as the loopback to
//! verify that received data is sent data.
//!
//!               MSP430F5438A
//!             -----------------
//!       RST -|     P3.4/UCA0TXD|----|
//!            |                 |    |
//!            |                 |    |
//!            |     P3.5/UCA0RXD|----|
//!            |                 |
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - UART peripheral
//! - GPIO Port peripheral (for UART pins)
//! - UCA0TXD
//! - UCA0RXD
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - USCI_A0_VECTOR.
//******************************************************************************
#include "driverlib.h"

//*****************************************************************************
//
//Select Baud rate
//
//*****************************************************************************
#define BAUD_RATE                               9600
//*****************************************************************************
//
//Initialize received data
//
//*****************************************************************************
uint8_t receivedData = 0x00;
//*****************************************************************************
//
//Initialize trasnmit data
//
//*****************************************************************************
uint8_t transmitData = 0x00;

uint8_t check = 0;

void main (void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    //P3.4,5 = USCI_A0 TXD/RXD
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P3,
        GPIO_PIN4 + GPIO_PIN5
        );

    //Baudrate = 9600, clock freq = 1.048MHz
    //UCBRx = 109, UCBRFx = 0, UCBRSx = 2, UCOS16 = 0
    USCI_A_UART_initParam param = {0};
    param.selectClockSource = USCI_A_UART_CLOCKSOURCE_SMCLK;
    param.clockPrescalar = 109;
    param.firstModReg = 0;
    param.secondModReg = 2;
    param.parity = USCI_A_UART_NO_PARITY;
    param.msborLsbFirst = USCI_A_UART_LSB_FIRST;
    param.numberofStopBits = USCI_A_UART_ONE_STOP_BIT;
    param.uartMode = USCI_A_UART_MODE;
    param.overSampling = USCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION;

    if (STATUS_FAIL == USCI_A_UART_init(USCI_A0_BASE, &param)){
        return;
    }

    //Enable UART module for operation
    USCI_A_UART_enable(USCI_A0_BASE);

    //Enable Receive Interrupt
	  USCI_A_UART_clearInterrupt(USCI_A0_BASE,
        USCI_A_UART_RECEIVE_INTERRUPT);
    USCI_A_UART_enableInterrupt(USCI_A0_BASE,
        USCI_A_UART_RECEIVE_INTERRUPT);

    __enable_interrupt();

    while (1)
    {
    	transmitData = transmitData+1;                      // Increment TX data
        // Load data onto buffer
        USCI_A_UART_transmitData(USCI_A0_BASE,
        		transmitData);
        while(check != 1);
        check = 0;
    }
}

//******************************************************************************
//
//This is the USCI_A0 interrupt vector service routine.
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_A0_VECTOR)))
#endif
void USCI_A0_ISR (void)
{
    switch (__even_in_range(UCA0IV,4)){
        //Vector 2 - RXIFG
        case 2:
            receivedData = USCI_A_UART_receiveData(USCI_A0_BASE);
            if(!(receivedData == transmitData))                   // Check value
            {
              while(1);
            }
            check =1;
            break;
        default: break;
    }
}


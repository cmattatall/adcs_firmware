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
#include "driverlib.h"

//*****************************************************************************
//! This example shows how SPI master talks to SPI slave using 3-wire mode.
//! Incrementing data is sent by the master starting at 0x01. Received data is
//! expected to be same as the previous transmission.  USCI RX ISR is used to
//! handle communication with the CPU, normally in LPM0. If high, P1.0 indicates
//! valid data reception.  Because all execution after LPM0 is in ISRs,
//! initialization waits for DCO to stabilize against ACLK.
//! ACLK = ~32.768kHz, MCLK = SMCLK = DCO ~ 1048kHz.  BRCLK = SMCLK/2
//!
//! Use with SPI Slave Data Echo code example.  If slave is in debug mode, P1.1
//! slave reset signal conflicts with slave's JTAG; to work around, use IAR's
//! "Release JTAG on Go" on slave device.  If breakpoints are set in
//! slave RX ISR, master must stopped also to avoid overrunning slave
//! RXBUF.
//!
//!                  MSP430F5438A
//!                 -----------------
//!            /|\ |                 |
//!             |  |                 |
//!    Master---+->|RST              |
//!                |                 |
//!                |             P3.1|-> Data Out (UCB0SIMO)
//!                |                 |
//!                |             P3.2|<- Data In (UCB0SOMI)
//!                |                 |
//!                |             P3.3|-> Serial Clock Out (UCB0CLK)
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - SPI peripheral
//! - GPIO Port peripheral (for SPI pins)
//! - UCB0SIMO
//! - UCB0SOMI
//! - UCB0CLK
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - USCI_A0_VECTOR
//!
//*****************************************************************************

//*****************************************************************************
//
//Specify desired frequency of SPI communication
//
//*****************************************************************************
#define SPICLK                          500000

uint8_t transmitData = 0x00, receiveData = 0x00;
uint8_t returnValue = 0x00;

void main (void)
{
    //Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    //Set P1.1 for slave reset
    GPIO_setOutputHighOnPin(

        GPIO_PORT_P1,
        GPIO_PIN1
        );

    //Set P1.1 for slave reset
    //Set P1.0 to output direction
    GPIO_setAsOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN0
        );

    //P3.5,4,0 option select
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P3,
        GPIO_PIN1 + GPIO_PIN2 + GPIO_PIN3
        );

    //Initialize Master
    USCI_B_SPI_initMasterParam param = {0};
    param.selectClockSource = USCI_B_SPI_CLOCKSOURCE_SMCLK;
    param.clockSourceFrequency = UCS_getSMCLK();
    param.desiredSpiClock = SPICLK;
    param.msbFirst = USCI_B_SPI_MSB_FIRST;
    param.clockPhase = USCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = USCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    returnValue =  USCI_B_SPI_initMaster(USCI_B0_BASE, &param);

    if (STATUS_FAIL == returnValue){
        return;
    }

    //Enable SPI module
    USCI_B_SPI_enable(USCI_B0_BASE);

    //Enable Receive interrupt
	USCI_B_SPI_clearInterrupt(USCI_B0_BASE, USCI_B_SPI_RECEIVE_INTERRUPT);
    USCI_B_SPI_enableInterrupt(USCI_B0_BASE, USCI_B_SPI_RECEIVE_INTERRUPT);

    //Now with SPI signals initialized, reset slave
    GPIO_setOutputLowOnPin(
        GPIO_PORT_P1,
        GPIO_PIN1
        );

    //LED On
    GPIO_setOutputHighOnPin(
        GPIO_PORT_P1,
        GPIO_PIN0
        );

    //Wait for slave to initialize
    __delay_cycles(100);

    //Initialize data values
    transmitData = 0x00;

    //USCI_A0 TX buffer ready?
    while (!USCI_B_SPI_getInterruptStatus(USCI_B0_BASE,
               USCI_B_SPI_TRANSMIT_INTERRUPT)) ;

    //Transmit Data to slave
    USCI_B_SPI_transmitData(USCI_B0_BASE, transmitData);

    //CPU off, enable interrupts
    __bis_SR_register(LPM0_bits + GIE);
}

//******************************************************************************
//
//This is the USCI_B0 interrupt vector service routine.
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_B0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_B0_VECTOR)))
#endif
void USCI_B0_ISR (void)
{
    switch (__even_in_range(UCB0IV,4)){
        //Vector 2 - RXIFG
        case 2:
            //USCI_A0 TX buffer ready?
            while (!USCI_B_SPI_getInterruptStatus(USCI_B0_BASE,
                       USCI_B_SPI_TRANSMIT_INTERRUPT)) ;

            receiveData = USCI_B_SPI_receiveData(USCI_B0_BASE);

            //Increment data
            transmitData++;

            //Send next value
            USCI_B_SPI_transmitData(USCI_B0_BASE,
            transmitData
            );

            //Delay between transmissions for slave to process information
            __delay_cycles(40);

            break;
        default: break;
    }
}


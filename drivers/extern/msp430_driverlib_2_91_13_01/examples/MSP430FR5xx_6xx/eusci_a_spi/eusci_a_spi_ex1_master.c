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
//*****************************************************************************
//! USCI_A0, SPI 3-Wire Master Incremented Data
//! This example shows how SPI master talks to SPI slave using 3-wire mode.
//! Incrementing data is sent by the master starting at 0x01. Received data is
//! expected to be same as the previous transmission.  eUSCI RX ISR is used to
//! handle communication with the CPU, normally in LPM0. If high, P1.0 indicates
//! valid data reception.  Because all execution after LPM0 is in ISRs,
//! initialization waits for DCO to stabilize against SMCLK.
//! ACLK = ~32.768kHz, MCLK = SMCLK = DCO ~ 1048kHz.  BRCLK = SMCLK/2
//!
//! Use with SPI Slave Data Echo code example.  If slave is in debug mode, P1.1
//! slave reset signal conflicts with slave's JTAG; to work around, use IAR's
//! "Release JTAG on Go" on slave device.  If breakpoints are set in
//! slave RX ISR, master must stopped also to avoid overrunning slave
//! RXBUF.
//!
//!               Tested on MSP430FR5969
//!                 -----------------
//!            /|\ |                 |
//!             |  |                 |
//!    Master---+->|RST              |
//!                |                 |
//!                |             P2.0|-> Data Out (UCA0SIMO)
//!                |                 |
//!                |             P2.1|<- Data In (UCA0SOMI)
//!                |                 |
//!                |             P1.5|-> Serial Clock Out (UCA0CLK)
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - SPI peripheral
//! - GPIO Port peripheral (for SPI pins)
//! - UCA0SIMO
//! - UCA0SOMI
//! - UCA0CLK
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - USCI_A0_VECTOR
//!
//*****************************************************************************

#include "driverlib.h"

uint8_t RXData = 0;
uint8_t TXData = 0;

void main(void)
{
    volatile uint16_t i;

    //Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    /*
    * Select Port 1
    * Set Pin 0 as output
    */
    GPIO_setAsOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN0
    );
    /*
    * Select Port 1
    * Set Pin 0 to output Low.
    */
    GPIO_setOutputLowOnPin(
        GPIO_PORT_P1,
        GPIO_PIN0
    );

    //Set DCO frequency to max DCO setting
    CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_3);
    //Select DCO as the clock source for SMCLK with no frequency divider
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /*
    * Select Port 1
    * Set Pin 5 to input Secondary Module Function, (UCA0CLK).
    */
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P1,
        GPIO_PIN5,
        GPIO_SECONDARY_MODULE_FUNCTION
    );

    /*
    * Select Port 2
    * Set Pin 0, 1 to input Secondary Module Function, (UCA0TXD/UCA0SIMO, UCA0RXD/UCA0SOMI).
    */
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P2,
        GPIO_PIN0 + GPIO_PIN1,
        GPIO_SECONDARY_MODULE_FUNCTION
    );

    /*
    * Disable the GPIO power-on default high-impedance mode to activate
    * previously configured port settings
    */
    PMM_unlockLPM5();

    //Initialize Master
    EUSCI_A_SPI_initMasterParam param = {0};
    param.selectClockSource = EUSCI_A_SPI_CLOCKSOURCE_SMCLK;
    param.clockSourceFrequency = CS_getSMCLK();
    param.desiredSpiClock = 500000;
    param.msbFirst = EUSCI_A_SPI_MSB_FIRST;
    param.clockPhase = EUSCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    param.spiMode = EUSCI_A_SPI_3PIN;
    EUSCI_A_SPI_initMaster(EUSCI_A0_BASE, &param);

    //Enable SPI module
    EUSCI_A_SPI_enable(EUSCI_A0_BASE);

    EUSCI_A_SPI_clearInterrupt(EUSCI_A0_BASE,
            EUSCI_A_SPI_RECEIVE_INTERRUPT);
    // Enable USCI_A0 RX interrupt
    EUSCI_A_SPI_enableInterrupt(EUSCI_A0_BASE,
        EUSCI_A_SPI_RECEIVE_INTERRUPT);

    //Wait for slave to initialize
    __delay_cycles(100);

    TXData = 0x1;                             // Holds TX data

    //USCI_A0 TX buffer ready?
    while (!EUSCI_A_SPI_getInterruptStatus(EUSCI_A0_BASE,
        EUSCI_A_SPI_TRANSMIT_INTERRUPT)) ;

    //Transmit Data to slave
    EUSCI_A_SPI_transmitData(EUSCI_A0_BASE, TXData);

    __bis_SR_register(LPM0_bits + GIE);      // CPU off, enable interrupts
    __no_operation();                       // Remain in LPM0
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_A0_VECTOR)))
#endif
void USCI_A0_ISR(void)
{
    switch(__even_in_range(UCA0IV, USCI_SPI_UCTXIFG))
    {
        case USCI_SPI_UCRXIFG:      // UCRXIFG
            //USCI_A0 TX buffer ready?
            while (!EUSCI_A_SPI_getInterruptStatus(EUSCI_A0_BASE,
                EUSCI_A_SPI_TRANSMIT_INTERRUPT));

            RXData = EUSCI_A_SPI_receiveData(EUSCI_A0_BASE);

            //Increment data
            TXData++;

            //Send next value
            EUSCI_A_SPI_transmitData(EUSCI_A0_BASE,
                TXData
                );

            //Delay between transmissions for slave to process information
            __delay_cycles(40);
            break;
        default:
            break;
    }
}


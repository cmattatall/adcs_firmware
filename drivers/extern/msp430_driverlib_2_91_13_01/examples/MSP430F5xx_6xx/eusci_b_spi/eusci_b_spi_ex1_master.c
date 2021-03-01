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
//! USCI_A0, SPI 3-Wire Master Incremented Data
//! This example shows how SPI master talks to SPI slave using 3-wire mode.
//! Incrementing data is sent by the master starting at 0x01. Received data is
//! expected to be same as the previous transmission.  eUSCI RX ISR is used to
//! handle communication with the CPU, normally in LPM0. If high, P1.0 indicates
//! valid data reception.  Because all execution after LPM0 is in ISRs,
//! initialization waits for DCO to stabilize against ACLK.
//! ACLK = ~32.768kHz, MCLK = SMCLK = DCO ~ 1048kHz.  BRCLK = SMCLK/2
//!
//! Use with eusci_spi_ex1_slave code example.  If slave is in debug mode, P1.1
//! slave reset signal conflicts with slave's JTAG; to work around, use IAR's
//! "Release JTAG on Go" on slave device.  If breakpoints are set in
//! slave RX ISR, master must stopped also to avoid overrunning slave
//! RXBUF.
//!
//                     MSP430F67791A
//!                 -----------------
//!            /|\ |                 |
//!             |  |                 |
//!    Master---+->|RST              |
//!                |                 |
//!                |             P2.6|-> Data Out (UCB0SIMO)
//!                |                 |
//!                |             P2.5|<- Data In (UCB0SOMI)
//!                |                 |
//!                |             P2.7|-> Serial Clock Out (UCB0CLK)
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - SPI peripheral
//! - GPIO Port peripheral (for SPI pins)
//! - UCS peripheral
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

uint8_t RXData = 0;
uint8_t TXData = 0;

void main(void)
{
    volatile uint16_t i;

    //Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    //Configure Pins for UCB0CLK, UCB0SIMO, UCB0SOMI
    //Set P2.5, P2.6, P2.7 as Module Function Input.
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P2,
            GPIO_PIN5 + GPIO_PIN6 + GPIO_PIN7
            );

    //Set the XT1 frequency to UCS
    UCS_setExternalClockSource(32768, 0);

    //Initialize Master
    EUSCI_B_SPI_initMasterParam param = {0};
    param.selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK;
    param.clockSourceFrequency = UCS_getSMCLK();
    param.desiredSpiClock = 500000;
    param.msbFirst = EUSCI_B_SPI_MSB_FIRST;
    param.clockPhase = EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    param.spiMode = EUSCI_B_SPI_3PIN;
    EUSCI_B_SPI_initMaster(EUSCI_B0_BASE, &param);

    //Enable SPI module
    EUSCI_B_SPI_enable(EUSCI_B0_BASE);

    EUSCI_B_SPI_clearInterrupt(EUSCI_B0_BASE,
            EUSCI_B_SPI_RECEIVE_INTERRUPT);
    // Enable USCI_B0 RX interrupt
    EUSCI_B_SPI_enableInterrupt(EUSCI_B0_BASE,
            EUSCI_B_SPI_RECEIVE_INTERRUPT);

    //Wait for slave to initialize
    __delay_cycles(100);

    TXData = 0x1;                             // Holds TX data

    //USCI_B0 TX buffer ready?
    while (!EUSCI_B_SPI_getInterruptStatus(EUSCI_B0_BASE,
                EUSCI_B_SPI_TRANSMIT_INTERRUPT)) ;

    //Transmit Data to slave
    EUSCI_B_SPI_transmitData(EUSCI_B0_BASE, TXData);

    __bis_SR_register(LPM0_bits + GIE);      // CPU off, enable interrupts
    __no_operation();                       // Remain in LPM0
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_B0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_B0_VECTOR)))
#endif
void USCI_B0_ISR (void)
{
    switch (__even_in_range(UCB0IV, USCI_SPI_UCTXIFG))
    {
        case USCI_SPI_UCRXIFG:      // UCRXIFG
            //USCI_B0 TX buffer ready?
            while (!EUSCI_B_SPI_getInterruptStatus(EUSCI_B0_BASE,
                        EUSCI_B_SPI_TRANSMIT_INTERRUPT));

            RXData = EUSCI_B_SPI_receiveData(EUSCI_B0_BASE);

            //Increment data
            TXData++;

            //Send next value
            EUSCI_B_SPI_transmitData(EUSCI_B0_BASE,
                    TXData
                    );

            //Delay between transmissions for slave to process information
            __delay_cycles(40);
            break;
        default:
            break;
    }
}


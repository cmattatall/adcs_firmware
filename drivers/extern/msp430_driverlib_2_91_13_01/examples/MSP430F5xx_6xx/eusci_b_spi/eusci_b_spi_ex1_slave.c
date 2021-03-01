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
//!   SPI slave talks to SPI master using 3-wire mode. Data is received
//!   from master and data from slave is then transmitted back to master.
//!   USCI RX ISR is used to handle communication, CPU normally in LPM4.
//!   Prior to initial data exchange, master pulses slaves RST for complete
//!   reset.
//!
//!   Use with eusci_spi_ex1_master code example.  If the slave is in
//!   debug mode, the reset signal from the master will conflict with slave's
//!   JTAG; to work around, use IAR's "Release JTAG on Go" on slave device.  If
//!   breakpoints are set in slave RX ISR, master must stopped also to avoid
//!   overrunning slave RXBUF.
//!
//!   Tesed on MSP430F67791A
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
//
//*****************************************************************************
uint8_t transmitData = 0x01, receiveData = 0x00;

void main(void)
{
    //Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    //Configure Pins for UCB0CLK, UCB0SIMO, UCB0SOMI
    //Set P2.5, P2.6, P2.7 as Module Function Input.
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P2,
            GPIO_PIN5 + GPIO_PIN6 + GPIO_PIN7
            );

    //Initialize slave to MSB first, inactive high clock polarity and 3 wire SPI
    EUSCI_B_SPI_initSlaveParam param = {0};
    param.msbFirst = EUSCI_B_SPI_MSB_FIRST;
    param.clockPhase = EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    param.spiMode = EUSCI_B_SPI_3PIN;
    EUSCI_B_SPI_initSlave(EUSCI_B0_BASE, &param);

    //Enable SPI Module
    EUSCI_B_SPI_enable(EUSCI_B0_BASE);

    EUSCI_B_SPI_clearInterrupt(EUSCI_B0_BASE,
            EUSCI_B_SPI_RECEIVE_INTERRUPT);
    //Enable Receive interrupt
    EUSCI_B_SPI_enableInterrupt(EUSCI_B0_BASE,
            EUSCI_B_SPI_RECEIVE_INTERRUPT
            );

    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, enable interrupts
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
    switch(__even_in_range(UCB0IV, USCI_SPI_UCTXIFG))
    {
        case USCI_SPI_UCRXIFG:      // UCRXIFG
            //USCI_B0 TX buffer ready?
            while (!EUSCI_B_SPI_getInterruptStatus(EUSCI_B0_BASE,
                        EUSCI_B_SPI_TRANSMIT_INTERRUPT
                        ));

            //Transmit data to master
            EUSCI_B_SPI_transmitData(EUSCI_B0_BASE,
                    transmitData
                    );

            //Receive data from master
            receiveData = EUSCI_B_SPI_receiveData(EUSCI_B0_BASE);

            //Increment data to be transmitted
            transmitData++;
            break;
        default:
            break;
    }
}


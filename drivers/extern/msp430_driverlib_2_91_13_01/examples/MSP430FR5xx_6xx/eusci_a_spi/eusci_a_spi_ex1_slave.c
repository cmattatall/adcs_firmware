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
//!   Tesed on        MSP430FR5969
//!                 -----------------
//!            /|\ |                 |
//!             |  |                 |
//!    Master---+->|RST              |
//!                |                 |
//!                |             P2.0|-> Data Out (UCA0SIMO)
//!                |                 |
//!                |             P2.1|<- Data In (UCA0SOMI)
//!                |                 |
//!                |             P1.5|<- Serial Clock Out (UCA0CLK)
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
//
//*****************************************************************************

#include "driverlib.h"

uint8_t transmitData = 0x01, receiveData = 0x00;

void main(void)
{
    //Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    // Configure Pins for LFXIN
    //Set PJ.4 and PJ.5 as Primary Module Function Input.
    /*

    * Select Port J
    * Set Pin 4, 5 to input Primary Module Function, (LFXIN).
    */
    GPIO_setAsPeripheralModuleFunctionInputPin(
    	GPIO_PORT_PJ,
    	GPIO_PIN4 + GPIO_PIN5,
    	GPIO_PRIMARY_MODULE_FUNCTION
    );

    // Configure SPI pins
    // Configure Pins for UCA0CLK
    //Set P1.5 as Secondary Module Function Input.
    /*

    * Select Port 1
    * Set Pin 5 to input Secondary Module Function, (UCA0CLK).
    */
    GPIO_setAsPeripheralModuleFunctionInputPin(
    	GPIO_PORT_P1,
    	GPIO_PIN5,
    	GPIO_SECONDARY_MODULE_FUNCTION
    );
    // Configure Pins for UCA0TXD/UCA0SIMO, UCA0RXD/UCA0SOMI
    //Set P2.0, P2.1 as Secondary Module Function Input.
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

    //Initialize slave to MSB first, inactive high clock polarity and 3 wire SPI
    EUSCI_A_SPI_initSlaveParam param = {0};
    param.msbFirst = EUSCI_A_SPI_MSB_FIRST;
    param.clockPhase = EUSCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    param.spiMode = EUSCI_A_SPI_3PIN;
    EUSCI_A_SPI_initSlave(EUSCI_A0_BASE, &param);

    //Enable SPI Module
    EUSCI_A_SPI_enable(EUSCI_A0_BASE);

    EUSCI_A_SPI_clearInterrupt(EUSCI_A0_BASE,
            EUSCI_A_SPI_RECEIVE_INTERRUPT);

    //Enable Receive interrupt
    EUSCI_A_SPI_enableInterrupt(EUSCI_A0_BASE,
    	  EUSCI_A_SPI_RECEIVE_INTERRUPT
          );

    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, enable interrupts
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
    switch(__even_in_range(UCA0IV, USCI_SPI_UCTXIFG))
    {
        case USCI_SPI_UCRXIFG:      // UCRXIFG
            //USCI_A0 TX buffer ready?
            while (!EUSCI_A_SPI_getInterruptStatus(EUSCI_A0_BASE,
                       EUSCI_A_SPI_TRANSMIT_INTERRUPT
                        ));

            //Transmit data to master
            EUSCI_A_SPI_transmitData(EUSCI_A0_BASE,
            transmitData
            );

            //Receive data from master
            receiveData = EUSCI_A_SPI_receiveData(EUSCI_A0_BASE);

            //Increment data to be transmitted
            transmitData++;
            break;
        default:
            break;
    }
}


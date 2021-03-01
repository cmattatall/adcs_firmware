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
//!  EUSCI_B0 I2C Slave TX multiple bytes to MSP430 Master
//!
//!  Description: This demo connects two MSP430's via the I2C bus. The master
//!  reads from the slave. This is the SLAVE code. The TX data begins at 0
//!  and is incremented each time it is sent. A stop condition
//!  is used as a trigger to initialize the outgoing data.
//!  The USCI_B0 TX interrupt is used to know
//!  when to TX.
//!	 Tested on MSP430FR5739, MSP430FR5969
//!
//!                                /|\  /|\
//!                                 10k  10k
//!                   slave         |    |        master
//!             -----------------   |    |   -----------------
//!           -|XIN  P1.6/UCB0SDA|<-|----+->|P1.6/UCB0SDA  XIN|-
//!            |                 |  |       |                 |
//!           -|XOUT             |  |       |             XOUT|-
//!            |     P1.7/UCB0SCL|<-+------>|P1.7/UCB0SCL     |
//!            |                 |          |                 |
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - I2C peripheral
//! - GPIO Port peripheral (for I2C pins)
//! - SCL2
//! - SDA
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - USCI_B0_VECTOR.
//!
//
//*****************************************************************************
//*****************************************************************************
//
//Set the address for slave module. This is a 7-bit address sent in the
//following format:
//[A6:A5:A4:A3:A2:A1:A0:RS]
//
//A zero in the "RS" position of the first byte means that the master
//transmits (sends) data to the selected slave, and a one in this position
//means that the master receives data from the slave.
//
//*****************************************************************************
#define SLAVE_ADDRESS 0x48

uint8_t TXData = 0;

void main(void)
{
	WDT_A_hold(WDT_A_BASE);

    // Configure Pins for I2C
	//Set P1.6 and P1.7 as Secondary Module Function Input.
	/*

	* Select Port 1
	* Set Pin 6, 7 to input Secondary Module Function, (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
	*/
	GPIO_setAsPeripheralModuleFunctionInputPin(
		GPIO_PORT_P1,
		GPIO_PIN6 + GPIO_PIN7,
		GPIO_SECONDARY_MODULE_FUNCTION
	);

    EUSCI_B_I2C_initSlaveParam param = {0};
    param.slaveAddress = 0x48;
    param.slaveAddressOffset = EUSCI_B_I2C_OWN_ADDRESS_OFFSET0;
    param.slaveOwnAddressEnable = EUSCI_B_I2C_OWN_ADDRESS_ENABLE;
    EUSCI_B_I2C_initSlave(EUSCI_B0_BASE, &param);

    EUSCI_B_I2C_enable(EUSCI_B0_BASE);

	EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE,
    		EUSCI_B_I2C_TRANSMIT_INTERRUPT0 +
    		EUSCI_B_I2C_STOP_INTERRUPT
    		);

    EUSCI_B_I2C_enableInterrupt(EUSCI_B0_BASE,
    		EUSCI_B_I2C_TRANSMIT_INTERRUPT0 +
    		EUSCI_B_I2C_STOP_INTERRUPT
    		);

    __bis_SR_register(CPUOFF + GIE);        // Enter LPM0 w/ interrupts
    __no_operation();
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_B0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_B0_VECTOR)))
#endif
void USCIB0_ISR(void)

{

switch(__even_in_range(UCB0IV,0x1E))
    {
        case 0x00: break; // Vector 0: No interrupts break;
        case 0x02: break; // Vector 2: ALIFG
        case 0x04: break; // Vector 4: NACKIFG
        case 0x06: break; // Vector 6: STTIFG
        case 0x08:       // Vector 8: STPIFG
          TXData = 0;
           break;
        case 0x0a: break; // Vector 10: RXIFG3 break;
        case 0x0c: break; // Vector 14: TXIFG3 break;
        case 0x0e: break; // Vector 16: RXIFG2 break;
        case 0x10: break; // Vector 18: TXIFG2 break;
        case 0x12: break; // Vector 20: RXIFG1 break;
        case 0x14: break; // Vector 22: TXIFG1 break;
        case 0x16: break; // Vector 24: RXIFG0 break;
        case 0x18:
            EUSCI_B_I2C_slavePutData(EUSCI_B0_BASE,
        		   TXData++);
           break; // Vector 26: TXIFG0 break;
        case 0x1a: break; // Vector 28: BCNTIFG break;
        case 0x1c: break; // Vector 30: clock low timeout break;
        case 0x1e: break; // Vector 32: 9th bit break;
        default: break;

    }
}

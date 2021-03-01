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

//******************************************************************************
//! This example shows how to configure the I2C module as a master for
//! multi byte transmission in interrupt driven mode. The address of the slave
//! module is set in this example.
//!
//!  MSP430FR57xx Demo - USCI_B0 I2C Master TX multiple bytes to MSP430 Slave
//!
//!  Description: This demo connects two MSP430's via the I2C bus. The master
//!  transmits to the slave. This is the MASTER CODE. It cntinuously
//!  transmits an array of data and demonstrates how to implement an I2C
//!  master transmitter sending multiple bytes using the USCI_B0 TX interrupt.
//!  ACLK = n/a, MCLK = SMCLK = BRCLK = default DCO = ~1.045MHz
//!
//!                                /|\  /|\
//!                MSP430FR5739    10k  10k      MSP430FR5739
//!                   slave         |    |         master
//!             -----------------   |    |   -----------------
//!           -|XIN  P1.6/UCB0SDA|<-|----+->|P1.6/UCB0SDA  XIN|-
//!            |                 |  |       |                 |
//!           -|XOUT             |  |       |             XOUT|-
//!            |     P1.7/UCB0SCL|<-+------>|P1.7/UCB0SCL     |
//!            |                 |          |                 |
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
//

//******************************************************************************
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

uint8_t TXData =0;                    // Pointer to TX data
uint8_t TXByteCtr;

void main(void)
{
	WDT_A_hold(WDT_A_BASE);

	//Set DCO frequency to 8MHz
	CS_setDCOFreq(CS_DCORSEL_0,CS_DCOFSEL_3);
	//Set ACLK = DCO with frequency divider of 8
	CS_initClockSignal(CS_ACLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_8);
	//Set SMCLK = DCO with frequency divider of 8
	CS_initClockSignal(CS_SMCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_8);
	//Set MCLK = DCO with frequency divider of 8
	CS_initClockSignal(CS_MCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_8);

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

    EUSCI_B_I2C_initMasterParam param = {0};
    param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = 1000000;
    param.dataRate = EUSCI_B_I2C_SET_DATA_RATE_400KBPS;
    param.byteCounterThreshold = 0;
    param.autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP;
    EUSCI_B_I2C_initMaster(EUSCI_B0_BASE, &param);

	//Specify slave address
	EUSCI_B_I2C_setSlaveAddress(EUSCI_B0_BASE,
				SLAVE_ADDRESS
				);

	//Set Master in receive mode
	EUSCI_B_I2C_setMode(EUSCI_B0_BASE,
				EUSCI_B_I2C_TRANSMIT_MODE
				);

	//Enable I2C Module to start operations
	EUSCI_B_I2C_enable(EUSCI_B0_BASE);

	EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE,
				EUSCI_B_I2C_TRANSMIT_INTERRUPT0 +
				EUSCI_B_I2C_NAK_INTERRUPT
				);

	//Enable master Receive interrupt
	EUSCI_B_I2C_enableInterrupt(EUSCI_B0_BASE,
				EUSCI_B_I2C_TRANSMIT_INTERRUPT0 +
				EUSCI_B_I2C_NAK_INTERRUPT
			  );
	while(1)
	{
	__delay_cycles(1000);                   // Delay between transmissions
	TXByteCtr = 4;                          // Load TX byte counter
	TXData = 0;

	while (EUSCI_B_I2C_SENDING_STOP == EUSCI_B_I2C_masterIsStopSent
			(EUSCI_B0_BASE));


	EUSCI_B_I2C_masterSendMultiByteStart(EUSCI_B0_BASE,
			TXData++);

	__bis_SR_register(CPUOFF + GIE);        // Enter LPM0 w/ interrupts
											// Remain in LPM0 until all data
											// is TX'd
											// Increment data byte
	}

}


//------------------------------------------------------------------------------
// The USCIAB0TX_ISR is structured such that it can be used to transmit any
// number of bytes by pre-loading TXByteCtr with the byte count. Also, TXData
// points to the next byte to transmit.
//------------------------------------------------------------------------------
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
		case 0x00: break;                    // Vector 0: No interrupts break;
		case 0x02: break;
		case 0x04:
			//resend start if NACK
			EUSCI_B_I2C_masterSendStart(EUSCI_B0_BASE);
		  break;                            // Vector 4: NACKIFG break;
		case 0x18:
	  if (TXByteCtr)                    // Check TX byte counter
		 {

		  EUSCI_B_I2C_masterSendMultiByteNext(EUSCI_B0_BASE,
				  TXData++);
			TXByteCtr--;                    // Decrement TX byte counter

		 }
		  else
		  {
			  EUSCI_B_I2C_masterSendMultiByteStop(EUSCI_B0_BASE);

			__bic_SR_register_on_exit(CPUOFF);// Exit LPM0
		  }
		  break;                            // Vector 26: TXIFG0 break;
		default: break;
  }
}

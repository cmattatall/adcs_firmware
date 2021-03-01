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
//!   Long word writes to FRAM
//!
//!   Description: Use long word write to write to 512 byte blocks of FRAM.
//!   Toggle LEDs after every 100 writes.
//!   NOTE: Running this example for extended periods will impact the FRAM
//!   endurance.
//!   ACLK = VLO, MCLK = SMCLK = 4MHz
//!
//!   Tested On: MSP430FR5739
//!         ---------------
//!     /|\|               |
//!      | |               |
//!      --|RST            |
//!        |               |
//!        |               |
//!        |           P1.0|---> LED
//!		   |           P3.5|-->LED
//!
//******************************************************************************
#include "driverlib.h"

#define FRAM_TEST_START 0xCABA

void main(void) {
	uint8_t count = 0;
	uint32_t data = 0;

	WDT_A_hold(WDT_A_BASE);                 // Stop WDT

	//Set P1.0 as an output pin.
	/*

	 * Select Port 1
	 * Set Pin 0 as output
	 */
	GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
	//Set P3.5 as an output pin.
	/*

	 * Select Port 3
	 * Set Pin 5 as output
	 */
	GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN5);

	// Initialize dummy data
	data = 0x11111111;

	while (1) {
		data += 0x00010001;

		FRAMCtl_memoryFill32(data, (uint32_t *) FRAM_TEST_START,
				128);
		count++;
		if (count > 100) {
			//Toggle P1.0 output pin.
			/*

			 * Select Port 1
			 * Set Pin 0 as output
			 */
			GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
			//Toggle P3.5 output pin.
			/*

			 * Select Port 3
			 * Set Pin 5 as output
			 */
			GPIO_toggleOutputOnPin(GPIO_PORT_P3, GPIO_PIN5);

			count = 0;                            //Reset counter
			data = 0x11111111;
		}
	}
}

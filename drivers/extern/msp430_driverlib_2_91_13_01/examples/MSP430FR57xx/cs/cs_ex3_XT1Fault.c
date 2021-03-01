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
//!   MSP430FR57xx Demo - Output 32768Hz crystal on XT1 and observe failsafe
//!
//!   Description: Configure ACLK = XT1 in LF mode and enter LPM3.
//!   To observe failsafe behavior short the crystal briefly on the target board.
//!   This will cause an NMI to occur. P1.0 and P3.5 is toggled inside the NMI ISR.
//!   Once the fault flag is cleared XT1 operation continues from 32768Hz crystal
//!   Otherwise ACLK defaults to VLO (~8KHz).
//!   ACLK = LFXT1 = 32kHz, MCLK = SMCLK = 4MHz
//!
//!           MSP430FR57x
//!         ---------------
//!     /|\|               |
//!      | |               |
//!      --|RST            |
//!        |               |
//!        |          P2.0 |--->ACLK
//!        |          P1.0 |--->LED
//!        |          P3.5 |--->LED
//!
//!
//!   Texas Instruments Inc.
//!   February 2012
//!   Built with IAR Embedded Workbench Version: 5.40 & Code Composer Studio V5.1
//******************************************************************************

//*****************************************************************************
//
//Variable to store status of Oscillator fault flags and obtain clock frequency
//
//
//*****************************************************************************
uint16_t status;
uint32_t clockValue;

void main(void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    // LED setup
    //Set P1.0 as an output pin.
    /*

     * Select Port 1
     * Set Pin 0 as output
     */
	GPIO_setAsOutputPin(
			GPIO_PORT_P1,
			GPIO_PIN0
	);

	//Set P3.5 as Output Low.
	/*

	* Select Port 3
	* Set Pin 5 to output Low.
	*/
	GPIO_setAsOutputPin(
		GPIO_PORT_P3,
		GPIO_PIN5
	);

	// XT1 Setup
	//Set PJ.4 and PJ.5 as Primary Module Function Input.
	/*

	* Select Port J
	* Set Pin 4, 5 to input Primary Module Function, XT1.
	*/
	GPIO_setAsPeripheralModuleFunctionInputPin(
		GPIO_PORT_PJ,
		GPIO_PIN4 + GPIO_PIN5,
		GPIO_PRIMARY_MODULE_FUNCTION
	);

  //Set DCO Frequency
  CS_setDCOFreq(CS_DCORSEL_0,CS_DCOFSEL_3);
  //Initialize ACLK=XT1 and no CLK DIVIDER set to 1
  CS_initClockSignal(CS_ACLK,CS_XT1CLK_SELECT,CS_CLOCK_DIVIDER_1);
  //Initialize SMCLK=DCOCLK and no CLK DIVIDER set to 2
  CS_initClockSignal(CS_SMCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_2);
  //Initialize MCLK=DCOCLK and no CLK DIVIDER set to 2
  CS_initClockSignal(CS_MCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_2);
  //Set XT1 frequency
  CS_setExternalClockSource(32768,0);
  //Start XT1 external crystal
  CS_turnOnXT1(CS_XT1_DRIVE_0);


  SFR_clearInterrupt(SFR_OSCILLATOR_FAULT_INTERRUPT);
  // Now that XT1OSC is running, enable fault interrupt
  SFR_enableInterrupt(SFR_OSCILLATOR_FAULT_INTERRUPT);

  //Set P2.0 as Output Low.
  /*

   * Select Port 2
   * Set Pin 0 to output Low.
   */
  GPIO_setOutputLowOnPin(
			GPIO_PORT_P2,
			GPIO_PIN0
	);

  //Set P2.0 as Ternary Module Function Output.
  /*

   * Select Port 2
   * Set Pin 0 to output Ternary Module Function, ACLK.
   */
  GPIO_setAsPeripheralModuleFunctionOutputPin(
			GPIO_PORT_P2,
			GPIO_PIN0,
			GPIO_TERNARY_MODULE_FUNCTION
	);


  CS_turnOffXT2();

    clockValue = CS_getSMCLK();
    clockValue = CS_getMCLK();
    clockValue = CS_getACLK();

  __bis_SR_register(LPM3_bits+GIE);

}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=UNMI_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(UNMI_VECTOR)))
#endif
void UNMI_ISR(void)
{
    do
  {
    // set a breakpoint on the line below to observe XT1 operating from VLO
    // when the breakpoint is hit during a crystal fault

	//Set P1.0 and P3.5 LED on
	/*

	* Select Port 1
	* Set Pin 0 to output high.
	*/
	GPIO_setOutputHighOnPin(
		GPIO_PORT_P1,
		GPIO_PIN0
	);

	/*

	* Select Port 3
	* Set Pin 5 to output high.
	*/
	GPIO_setOutputHighOnPin(
		GPIO_PORT_P3,
		GPIO_PIN5
		);

    status=CS_clearAllOscFlagsWithTimeout(1000);

  } while (status);

    //Clear P1.0 LED off
	/*

	* Select Port 1
	* Set Pin 0 to output Low.
	*/
	GPIO_setOutputLowOnPin(
		GPIO_PORT_P1,
		GPIO_PIN0
		);

    //Clear P3.5 LED off
	/*

	* Select Port 3
	* Set Pin 5 to output Low.
	*/
	GPIO_setOutputLowOnPin(
		GPIO_PORT_P3,
		GPIO_PIN5
	);

}



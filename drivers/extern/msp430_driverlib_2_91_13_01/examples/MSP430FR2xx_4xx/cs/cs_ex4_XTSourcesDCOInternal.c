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
//!  This program demonstrates using an external 32kHz crystal to supply ACLK,
//!  and using a high speed crystal or resonator to supply SMCLK.
//!  SMLCK for the CPU is supplied by the internal DCO.  The 32kHz crystal
//!  connects between pins Xin and Xout. The DCO clock is
//!  generated internally and calibrated from the 32kHz crystal.
//!  ACLK is brought out on pin P1.1, SMCLK is brought out on P1.0.
//!
//!  ACLK = XT1 = 32768Hz, SMCLK = default DCO = 32 x ACLK = 1048576Hz
//!
//!   An external watch crystal between XIN & XOUT is required for ACLK
//!
//!                MSP430FR2311
//!             -----------------
//!        /|\ |         XIN P2.6|-
//!         |  |                 | 32kHz
//!         ---|RST     XOUT P2.7|-
//!            |                 |
//!            |                 |
//!            |             P1.0|-->SMCLK = 1MHz
//!            |             P1.1|-->ACLK = 32kHz Crystal Out
//!            |                 |
//!            |                 |
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - CS module
//! - GPIO Port peripheral
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - None.
//!
//*****************************************************************************
#include "driverlib.h"

//*****************************************************************************
//
//Desired Timeout for XT1 initialization
//
//*****************************************************************************
#define CS_XT1_TIMEOUT 50000

//*****************************************************************************
//
//Desired Timeout for XT2 initialization
//
//*****************************************************************************
#define CS_XT2_TIMEOUT 0

//*****************************************************************************
//
//XT1 Crystal Frequency being used
//
//*****************************************************************************
#define CS_XT1_CRYSTAL_FREQUENCY    32768

//*****************************************************************************
//
//Variable to store returned STATUS_SUCCESS or STATUS_FAIL
//
//*****************************************************************************
uint8_t returnValue = 0;

//*****************************************************************************
//
//Variable to store current clock values
//
//*****************************************************************************
uint32_t clockValue;

//*****************************************************************************
//
//Variable to store status of Oscillator fault flags
//
//*****************************************************************************
uint16_t status;

void main (void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    //Set P1.1 to output ACLK
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN1,
        GPIO_SECONDARY_MODULE_FUNCTION
        );

    //Set P1.0 to output SMCLK
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN0,
        GPIO_SECONDARY_MODULE_FUNCTION
        );

    //Port select XT1
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P2,
        GPIO_PIN6 + GPIO_PIN7,
        GPIO_SECONDARY_MODULE_FUNCTION
    );

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    //Initializes the XT1 and XT2 crystal frequencies being used
    CS_setExternalClockSource(
        CS_XT1_CRYSTAL_FREQUENCY
        );

    //Initialize XT1. Returns STATUS_SUCCESS if initializes successfully
    returnValue = CS_turnOnXT1LFWithTimeout(
        CS_XT1_DRIVE_0,
        CS_XT1_TIMEOUT
        );

    //Select XT1 as ACLK source
    CS_initClockSignal(
        CS_ACLK,
        CS_XT1CLK_SELECT,
        CS_CLOCK_DIVIDER_1
        );

	//clear all OSC fault flag
	CS_clearAllOscFlagsWithTimeout(1000);

	//Enable oscillator fault interrupt
    SFR_enableInterrupt(SFR_OSCILLATOR_FAULT_INTERRUPT);

    // Enable global interrupt
    __bis_SR_register(GIE);

    //Verify if the Clock settings are as expected
    clockValue = CS_getMCLK();
    clockValue = CS_getACLK();
    clockValue = CS_getSMCLK();

    //Loop in place
    while (1) ;
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=UNMI_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(UNMI_VECTOR)))
#endif
void NMI_ISR(void)
{
  do {
    // If it still can't clear the oscillator fault flags after the timeout,
    // trap and wait here.
    status = CS_clearAllOscFlagsWithTimeout(1000);
  } while(status != 0);
}

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
//!  MLCK for the CPU is supplied by the internal DCO.  The 32kHz crystal
//!  connects between pins Xin and Xout. The high frequency crystal or
//!  resonator connects between pins XT2IN and XT2OUT.  The DCO clock is
//!  generated internally and calibrated from the 32kHz crystal.
//!  ACLK is brought out on pin P11.0, MCLK is brought out on P11.1,
//!  SMCLK is brought out on P11.2.
//!
//!  ACLK = LFXT1 = 32768Hz, MCLK = default DCO = 32 x ACLK = 1048576Hz
//!  SMCLK = HF XTAL
//!   An external watch crystal between XIN & XOUT is required for ACLK
//!
//!                MSP430F54xA
//!             -----------------
//!        /|\ |              XIN|-
//!         |  |                 | 32kHz
//!         ---|RST          XOUT|-
//!            |                 |
//!            |                 |
//!            |            XT2IN|-
//!            |                 | HF XTAL or Resonator (add capacitors)
//!            |           XT2OUT|-
//!            |                 |
//!            |            P11.0|-->ACLK = 32kHz Crystal Out
//!            |                 |
//!            |            P11.1|-->MCLK
//!            |                 |
//!            |            P11.2|-->SMCLK = High Freq Xtal or Resonator Out
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - UCS module
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
#define UCS_XT1_TIMEOUT 50000

//*****************************************************************************
//
//Desired Timeout for XT2 initialization
//
//*****************************************************************************
#define UCS_XT2_TIMEOUT 50000

//*****************************************************************************
//
//XT1 Crystal Frequency being used
//
//*****************************************************************************
#define UCS_XT1_CRYSTAL_FREQUENCY    32768

//*****************************************************************************
//
//XT2 Crystal Frequency being used
//
//*****************************************************************************
#define UCS_XT2_CRYSTAL_FREQUENCY   24000000

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

    //ACLK, MCLK, SMCLk set out to pins
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_P11,
        GPIO_PIN0 + GPIO_PIN1 + GPIO_PIN2
        );

    //Port select XT1
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P7,
        GPIO_PIN0 + GPIO_PIN1
        );

    //Initializes the XT1 and XT2 crystal frequencies being used
    UCS_setExternalClockSource(
        UCS_XT1_CRYSTAL_FREQUENCY,
        UCS_XT2_CRYSTAL_FREQUENCY
        );

    //Initialize XT1. Returns STATUS_SUCCESS if initializes successfully
    returnValue = UCS_turnOnLFXT1WithTimeout(
        UCS_XT1_DRIVE_0,
        UCS_XCAP_3,
        UCS_XT1_TIMEOUT
        );

    //Startup HF XT2 crystal Port select XT2
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P5,
        GPIO_PIN2 + GPIO_PIN3
        );

    //Initialize XT2. Returns STATUS_SUCCESS if initializes successfully
    returnValue = UCS_turnOnXT2WithTimeout(
        UCS_XT2_DRIVE_4MHZ_8MHZ,
        UCS_XT2_TIMEOUT
        );

    //Select XT1 as ACLK source
    UCS_initClockSignal(
        UCS_ACLK,
        UCS_XT1CLK_SELECT,
        UCS_CLOCK_DIVIDER_1
        );

    //Select XT2 as SMCLK source
    UCS_initClockSignal(
        UCS_SMCLK,
        UCS_XT2CLK_SELECT,
        UCS_CLOCK_DIVIDER_1
        );

    // Enable global oscillator fault flag
	SFR_clearInterrupt(SFR_OSCILLATOR_FAULT_INTERRUPT);
    SFR_enableInterrupt(SFR_OSCILLATOR_FAULT_INTERRUPT);

    // Enable global interrupt
    __bis_SR_register(GIE);

    //Verify if the Clock settings are as expected
    clockValue = UCS_getMCLK();
    clockValue = UCS_getACLK();
    clockValue = UCS_getSMCLK();

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
    status = UCS_clearAllOscFlagsWithTimeout(1000);
  } while(status != 0);
}

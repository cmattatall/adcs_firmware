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
//! Software Trim on 1MHz DCO
//!
//!   Performs proper software trim both from uncalculated trim values
//!   and pre-loaded trim values, and then toggles LED2
//!   ACLK, SMCLK, and MCLK are brought out to external pins on the board.
//!
//!   ACLK = REFO = ~32kHz, MCLK = SMCLK = 1MHz
//!
//!           MSP430FR2xx_4xx Board
//!             -----------------
//!         /|\|                 |
//!          | |                 |-->ACLK
//!          --|RST              |-->MCLK
//!            |                 |-->SMCLK
//!            |                 |
//!            |                 |-->LED2
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
//******************************************************************************
#include "driverlib.h"
#include "Board.h"

//*****************************************************************************
//
//Target frequency for MCLK in kHz
//
//*****************************************************************************
#define CS_MCLK_DESIRED_FREQUENCY_IN_KHZ   1000

//*****************************************************************************
//
//MCLK/FLLRef Ratio
//
//*****************************************************************************
#define CS_MCLK_FLLREF_RATIO   30

//*****************************************************************************
//
//Variable to store current Clock values
//
//*****************************************************************************
uint32_t clockValue = 0;

//*****************************************************************************
//
//Variable to store status of Oscillator fault flags
//
//*****************************************************************************
uint16_t status;

void main (void)
{
    //Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    //Set LED2 to output direction
    GPIO_setAsOutputPin(
        GPIO_PORT_LED2,
        GPIO_PIN_LED2
        );

    //ACLK set out to pins
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_ACLK,
        GPIO_PIN_ACLK,
        GPIO_FUNCTION_ACLK
        );
    //SMCLK set out to pins
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_SMCLK,
        GPIO_PIN_SMCLK,
        GPIO_FUNCTION_SMCLK
        );
    //MCLK set out to pins
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_MCLK,
        GPIO_PIN_MCLK,
        GPIO_FUNCTION_MCLK
        );

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    //Set DCO FLL reference = REFO
    CS_initClockSignal(
        CS_FLLREF,
        CS_REFOCLK_SELECT,
        CS_CLOCK_DIVIDER_1
        );

    //Set ACLK = REFO
    CS_initClockSignal(
        CS_ACLK,
        CS_REFOCLK_SELECT,
        CS_CLOCK_DIVIDER_1
        );
    
    //Create struct variable to store proper software trim values
    CS_initFLLParam param = {0};
    
    //Set Ratio/Desired MCLK Frequency, initialize DCO, save trim values
    CS_initFLLCalculateTrim(
        CS_MCLK_DESIRED_FREQUENCY_IN_KHZ,
        CS_MCLK_FLLREF_RATIO,
        &param
        );

    //Clear all OSC fault flag
    CS_clearAllOscFlagsWithTimeout(1000);

    //For demonstration purpose, change DCO clock freq to 16MHz
    CS_initFLLSettle(
        16000,
        487
        );

    //Clear all OSC fault flag
    CS_clearAllOscFlagsWithTimeout(1000);

    //Reload DCO trim values that were calculated earlier
    CS_initFLLLoadTrim(
        CS_MCLK_DESIRED_FREQUENCY_IN_KHZ,
        CS_MCLK_FLLREF_RATIO,
        &param
        );

    //Clear all OSC fault flag
    CS_clearAllOscFlagsWithTimeout(1000);

    //Enable oscillator fault interrupt
    SFR_enableInterrupt(SFR_OSCILLATOR_FAULT_INTERRUPT);

    // Enable global interrupt
    __bis_SR_register(GIE);

    //Verify if the Clock settings are as expected
    clockValue = CS_getSMCLK();
    clockValue = CS_getMCLK();
    clockValue = CS_getACLK();

    while (1)
    {
        //Toggle LED2
        GPIO_toggleOutputOnPin(
            GPIO_PORT_LED2,
            GPIO_PIN_LED2
            );

        //Delay
        __delay_cycles(1000000);
    }
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

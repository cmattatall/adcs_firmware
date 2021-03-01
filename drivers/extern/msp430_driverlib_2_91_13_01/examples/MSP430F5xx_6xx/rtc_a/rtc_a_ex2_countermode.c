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
//!
//! RTC_A in Counter Mode toggles P1.0 every 1s
//!
//!  This program demonstrates RTC in counter mode configured to source from ACLK
//!  to toggle P1.0 LED every 1s.
//!
//!                MSP430F5438A
//!             -----------------
//!        /|\ |                 |
//!         |  |                 |
//!         ---|RST              |
//!            |                 |
//!            |             P1.0|-->LED
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - RTC peripheral
//! - GPIO Port peripheral
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - RTC_A_VECTOR
//!
//******************************************************************************
#include "driverlib.h"


void main (void)
{
    WDT_A_hold(WDT_A_BASE);

    //Set P1.0 LED on
    GPIO_setOutputHighOnPin(
        GPIO_PORT_P1,
        GPIO_PIN0
        );

    //Set P1.0 to output direction
    GPIO_setAsOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN0
        );

    //Initialize Counter Mode of RTC_A_A
    /*
     * Base Address of the RTC_A_A
     * Use Prescaler 1 as source for counter
     * Specify counter as 8 bits, which asserts an interrupt for an overflow
     */
    RTC_A_initCounter(RTC_A_BASE,
        RTC_A_CLOCKSELECT_RT1PS,
        RTC_A_COUNTERSIZE_8BIT);

    //Initialize Prescalers
    /*
     * Base Address of the RTC_A_A
     * Specify Initialization of Prescaler 0
     * Use ACLK as source to prescaler
     * Divide source by 8 for this prescaler
     */
    RTC_A_initCounterPrescale(RTC_A_BASE,
        RTC_A_PRESCALE_0,
        RTC_A_PSCLOCKSELECT_ACLK,
        RTC_A_PSDIVIDER_8);

    /*
     * Base Address of the RTC_A_A
     * Specify Initialization of Prescaler 1
     * Use Prescaler 0 as source to prescaler
     * Divide source by 16 for this prescaler
     */
    RTC_A_initCounterPrescale(RTC_A_BASE,
        RTC_A_PRESCALE_1,
        RTC_A_PSCLOCKSELECT_RT0PS,
        RTC_A_PSDIVIDER_16);

    //Enable interrupt for counter overflow
	RTC_A_clearInterrupt(RTC_A_BASE,
        RTCTEVIFG);
    RTC_A_enableInterrupt(RTC_A_BASE,
        RTCTEVIE);

    //Start RTC Clock
    RTC_A_startClock(RTC_A_BASE);

    __bis_SR_register(LPM3_bits + GIE);
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=RTC_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(RTC_VECTOR)))
#endif
void RTC_A_ISR (void)
{
    switch (__even_in_range(RTCIV,16)){
        case 0: break;  //No interrupts
        case 2: break;  //RTCRDYIFG
        case 4:         //RTCEVIFG
            //Toggle P1.0
            GPIO_toggleOutputOnPin(
            GPIO_PORT_P1,
            GPIO_PIN0);
            break;
        case 6: break;  //RTCAIFG
        case 8: break;  //RT0PSIFG
        case 10: break; //RT1PSIFG
        default: break;
    }
}


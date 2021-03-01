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
//! RTC in Counter Mode toggles LED1 every 1s
//!
//!  This program demonstrates RTC in counter mode configured to source from SMCLK
//!  to toggle LED1 every 1s.
//!  SMCLK = REFO = ~32kHz
//!
//!           MSP430FR2xx_4xx Board
//!             -----------------
//!        /|\ |                 |
//!         |  |                 |
//!         ---|RST              |
//!            |                 |
//!            |                 |-->LED1
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
//! - RTC_VECTOR
//!
//******************************************************************************
#include "driverlib.h"
#include "Board.h"

//*****************************************************************************
//
//Interval time used for RTC
//
//*****************************************************************************
#define INTERVAL_TIME 32768

void main (void)
{
    WDT_A_hold(WDT_A_BASE);

    //Set LED1 to output direction
    GPIO_setAsOutputPin(
        GPIO_PORT_LED1,
        GPIO_PIN_LED1
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

    //Set SMCLK = REFO
    CS_initClockSignal(
        CS_SMCLK,
        CS_REFOCLK_SELECT,
        CS_CLOCK_DIVIDER_1
        );

    //Initialize RTC
    RTC_init(RTC_BASE,
        INTERVAL_TIME,
        RTC_CLOCKPREDIVIDER_1);

    RTC_clearInterrupt(RTC_BASE,
        RTC_OVERFLOW_INTERRUPT_FLAG);

    //Enable interrupt for RTC overflow
    RTC_enableInterrupt(RTC_BASE,
        RTC_OVERFLOW_INTERRUPT);

    //Start RTC Clock with clock source SMCLK
    RTC_start(RTC_BASE, RTC_CLOCKSOURCE_SMCLK);

    //Enter LPM3 mode with interrupts enabled
    __bis_SR_register(LPM0_bits + GIE);
    __no_operation();
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=RTC_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(RTC_VECTOR)))
#endif
void RTC_ISR (void)
{
    switch (__even_in_range(RTCIV,2)){
        case 0: break;  //No interrupts
        case 2:         //RTC overflow
            //Toggle LED1
            GPIO_toggleOutputOnPin(
            GPIO_PORT_LED1,
            GPIO_PIN_LED1);
            break;
        default: break;
    }
}

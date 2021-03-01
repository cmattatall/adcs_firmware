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
//! WDT - Toggle LED1, Interval Overflow ISR, 32kHz ACLK
//!
//!  Toggle LED1 using software timed by WDT ISR. Toggle rate is exactly 250ms
//!  based on 32kHz ACLK WDT clock source. In this example the WDT is configured
//!  to divide 32768 watch-crystal(2^15) by 2^13 with an ISR triggered
//!  @ 4Hz = [WDT CLK source/32768].
//!  ACLK = REFO , MCLK = SMCLK = default DCO ~1.048MHz
//!
//!           MSP430FR2xx_4xx Board
//!             -----------------
//!         /|\|                 |
//!          | |                 |
//!          --|RST              |
//!            |                 |-->LED1
//!            |                 |
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - WDT peripheral
//! - GPIO Port peripheral
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - WDT_A_VECTOR
//!
//******************************************************************************
#include "driverlib.h"
#include "Board.h"

void main (void)
{
    //Initialize WDT module in timer interval mode,
    //with ACLK as source at an interval of 250 ms.
    WDT_A_initIntervalTimer(WDT_A_BASE,
        WDT_A_CLOCKSOURCE_ACLK,
        WDT_A_CLOCKDIVIDER_8192);

	WDT_A_start(WDT_A_BASE);

    //Enable Watchdog Interupt
	SFR_clearInterrupt(SFR_WATCHDOG_INTERVAL_TIMER_INTERRUPT);
    SFR_enableInterrupt(SFR_WATCHDOG_INTERVAL_TIMER_INTERRUPT);

    //Set LED1 to output direction
    GPIO_setAsOutputPin(
        GPIO_PORT_LED1,
        GPIO_PIN_LED1
        );

    PMM_unlockLPM5();

    //Enter LPM3, enable interrupts
    __bis_SR_register(LPM3_bits + GIE);
    //For debugger
    __no_operation();
}

//Watchdog Timer interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=WDT_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(WDT_VECTOR)))
#endif
void WDT_A_ISR (void)
{
    //Toggle LED1
    GPIO_toggleOutputOnPin(
        GPIO_PORT_LED1,
        GPIO_PIN_LED1
        );
}


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
//*******************************************************************************
//!  Timer_B, PWM TB0.1, Up Mode, DCO SMCLK
//!
//!  Description: This program generates PWM outputs on P2.6 using
//!  Timer_B configured for up mode. The value , TIMER_PERIOD, defines the PWM
//!  period and the value DUTY_CYCLE the PWM duty cycle. Using ~1.045MHz
//!  SMCLK as TBCLK, the timer period is ~488us with a 75% duty cycle on P2.6
//!  ACLK = n/a, SMCLK = MCLK = TBCLK = default DCO ~1.045MHz.
//!
//!  Tested On: MSP430FR2311
//!            -------------------
//!        /|\|                   |
//!         | |                   |
//!         --|RST                |
//!           |                   |
//!           |         P1.6/TB0.1|--> CCR1 - 75% PWM
//!           |                   |
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - Timer peripheral
//! - GPIO Port peripheral
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - NONE
//******************************************************************************

#include "driverlib.h"

#define TIMER_PERIOD 511
#define DUTY_CYCLE  383

void main (void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    //P1.6 as PWM output
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN6,
        GPIO_SECONDARY_MODULE_FUNCTION
        );

    //Disable the GPIO power-on default high-impedance mode to activate
    //previously configured port settings
    PMM_unlockLPM5();

    //Generate PWM - Timer runs in Up mode
    Timer_B_outputPWMParam param = {0};
    param.clockSource = TIMER_B_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1;
    param.timerPeriod = TIMER_PERIOD;
    param.compareRegister = TIMER_B_CAPTURECOMPARE_REGISTER_1;
    param.compareOutputMode = TIMER_B_OUTPUTMODE_RESET_SET;
    param.dutyCycle = DUTY_CYCLE;
    Timer_B_outputPWM(TIMER_B0_BASE, &param);

    //Enter LPM0
    __bis_SR_register(LPM0_bits);

    //For debugger
    __no_operation();
}

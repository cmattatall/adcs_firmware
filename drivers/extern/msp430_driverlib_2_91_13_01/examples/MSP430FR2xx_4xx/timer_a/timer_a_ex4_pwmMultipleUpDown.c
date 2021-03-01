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
//!Timer_A3, PWM TA1.1-2, Up/Down Mode, DCO SMCLK
//!
//!Description: This program generates two PWM outputs on P4.0/8.3 using
//!Timer1_A configured for up/down mode. The value in CCR0, 128, defines the
//!PWM period/2 and the values in CCR1 and CCR2 the PWM duty cycles. Using
//!~1.045MHz SMCLK as TACLK, the timer period is ~233us with a 75% duty cycle
//!on P4.0 and 25% on P8.3.
//!SMCLK = MCLK = TACLK = default DCO ~1.048MHz.
//!
//!Tested On: MSP430FR4133
//!     -------------------
//! /|\|                   |
//!  | |                   |
//!  --|RST                |
//!    |                   |
//!    |         P4.0/TA1.1|--> CCR1 - 75% PWM
//!    |         P8.3/TA1.2|--> CCR2 - 25% PWM
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - Timer peripheral
//! - GPIO peripheral
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - NONE
//!
//
//*****************************************************************************
#include "driverlib.h"

#define TIMER_PERIOD 127
#define DUTY_CYCLE1 32
#define DUTY_CYCLE2 96

void main (void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    //P4.0 as output with module function
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_P4,
        GPIO_PIN0,
        GPIO_PRIMARY_MODULE_FUNCTION
        );

    //P8.3 as output with module function
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_P8,
        GPIO_PIN3,
        GPIO_PRIMARY_MODULE_FUNCTION
        );

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    //Start Timer
    Timer_A_initUpDownModeParam initUpDownParam = {0};
    initUpDownParam.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    initUpDownParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    initUpDownParam.timerPeriod = TIMER_PERIOD;
    initUpDownParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    initUpDownParam.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    initUpDownParam.timerClear = TIMER_A_DO_CLEAR;
    initUpDownParam.startTimer = false;
    Timer_A_initUpDownMode(TIMER_A1_BASE, &initUpDownParam);

    Timer_A_startCounter( TIMER_A1_BASE,
        TIMER_A_UPDOWN_MODE
        );

    //Initialze compare registers to generate PWM1
    Timer_A_initCompareModeParam initComp1Param = {0};
    initComp1Param.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    initComp1Param.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp1Param.compareOutputMode = TIMER_A_OUTPUTMODE_TOGGLE_SET;
    initComp1Param.compareValue = DUTY_CYCLE1;
    Timer_A_initCompareMode(TIMER_A1_BASE, &initComp1Param);

    //Initialze compare registers to generate PWM2
    Timer_A_initCompareModeParam initComp2Param = {0};
    initComp2Param.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;
    initComp2Param.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp2Param.compareOutputMode = TIMER_A_OUTPUTMODE_TOGGLE_SET;
    initComp2Param.compareValue = DUTY_CYCLE2;
    Timer_A_initCompareMode(TIMER_A1_BASE, &initComp2Param);

    //Enter LPM0
    __bis_SR_register(LPM0_bits);

    //For debugger
    __no_operation();
}


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
//  MSP430F51x2 Demo - TimerD0, PWM TD0.1-2, Up/Down Mode, DCO SMCLK
//
//  Description: This program generates two PWM outputs on P1.7 and P2.0 using
//  TimerD0 configured for up/down mode. The value in CCR0, 128, defines the
//  PWM period/2 and the values in CCR1 and CCR2 the PWM duty cycles. Using
//  ~1.045MHz SMCLK as TDCLK, the timer period is ~233us with a 75% duty cycle
//  on P1.7 and 25% on P2.0. PWM freq = 4kHz.
//  SMCLK = MCLK = TDCLK = default DCO ~1.045MHz.
//
//                MSP430F51x2
//            -------------------
//        /|\|                   |
//         | |                   |
//         --|RST                |
//           |                   |
//           |         P1.7/TD0.1|--> CCR1 - 75% PWM
//           |         P2.0/TD0.2|--> CCR2 - 25% PWM
//
//******************************************************************************
#include "driverlib.h"

void main(void)
{
    WDT_A_hold(WDT_A_BASE);
    // configure ports
    GPIO_setAsPeripheralModuleFunctionOutputPin(
    		  GPIO_PORT_P1,
    		  GPIO_PIN7
    		  );

    GPIO_setAsPeripheralModuleFunctionOutputPin(
    		  GPIO_PORT_P2,
    		  GPIO_PIN0
    		  );

    Timer_D_initCompareModeParam initComp1Param = {0};
    initComp1Param.compareRegister = TIMER_D_CAPTURECOMPARE_REGISTER_1;
    initComp1Param.compareInterruptEnable = TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp1Param.compareOutputMode = TIMER_D_OUTPUTMODE_TOGGLE_SET;
    initComp1Param.compareValue = 32;
    Timer_D_initCompareMode(TIMER_D0_BASE, &initComp1Param);

    Timer_D_initCompareModeParam initComp2Param = {0};
    initComp2Param.compareRegister = TIMER_D_CAPTURECOMPARE_REGISTER_2;
    initComp2Param.compareInterruptEnable = TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp2Param.compareOutputMode = TIMER_D_OUTPUTMODE_TOGGLE_SET;
    initComp2Param.compareValue = 96;
    Timer_D_initCompareMode(TIMER_D0_BASE, &initComp2Param);

    Timer_D_initUpDownModeParam initUpDownParam = {0};
    initUpDownParam.clockSource = TIMER_D_CLOCKSOURCE_SMCLK;
    initUpDownParam.clockSourceDivider = TIMER_D_CLOCKSOURCE_DIVIDER_1;
    initUpDownParam.clockingMode = TIMER_D_CLOCKINGMODE_EXTERNAL_CLOCK;
    initUpDownParam.timerPeriod = 128;
    initUpDownParam.timerInterruptEnable_TDIE = TIMER_D_TDIE_INTERRUPT_DISABLE;
    initUpDownParam.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initUpDownParam.timerClear = TIMER_D_DO_CLEAR;
    Timer_D_initUpDownMode(TIMER_D0_BASE, &initUpDownParam);

    Timer_D_startCounter(
      		  TIMER_D0_BASE,
      		  TIMER_D_UP_MODE
      		  );

    __bis_SR_register(LPM0_bits);             // Enter LPM0
    __no_operation();                         // For debugger
}


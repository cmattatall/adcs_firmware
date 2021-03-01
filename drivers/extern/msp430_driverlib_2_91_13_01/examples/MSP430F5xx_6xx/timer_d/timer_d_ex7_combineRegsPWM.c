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
//  MSP430F51x2 Demo - Timer0_D3, Combining two CCRx register to control one
//                     PWM channel; Normal Timer mode
//
//  Description: This code example shows how to combine two CCRx registers to
//  control both the rising and falling edges of one PWM channel (TD0.2, TD1.2).
//  In up-mode, TDxCCR1 controls the rising edge and TDxCCR2 controls the
//  falling edge. In this example, since CCR1 registers of TD0/1 have the same
//  count, the rising edges at TD0.2 and TD1.2 happen almost at the same
//  time instance. With TD0CCR2=40 and TD1CCR2=96, the dutycycles of the TD0.2
//  and TD1.2 are 30% and 43.75%.
//
//  ACLK = REF0; SMCLK = MCLK = default DCO ~1.045MHz.
//
//                 MSP430F51x2
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |       P2.0/TD0.2|--> CCR2 - 30% duty cycle;
//            |                 |    ON period=(TD0CCR2-TD0CCR1)/32767 ~ 1.22ms
//            |       P2.3/TD1.2|--> CCR2 - 43.75% duty cycle;
//            |                 |    ON period=(TD1CCR2-TD1CCR1)/32767 ~ 1.7ms
//
//******************************************************************************
#include "driverlib.h"

void main(void)
{
    WDT_A_hold(WDT_A_BASE);

    // Set port pins
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        		  GPIO_PORT_P2,
        		  GPIO_PIN0 + GPIO_PIN3
        		  );


    // Configure TimerD0 to combine CCR0/1 block to control TD0CCR2 PWM output
    Timer_D_combineTDCCRToOutputPWMParam pwm1Param = {0};
    pwm1Param.clockSource = TIMER_D_CLOCKSOURCE_SMCLK;
    pwm1Param.clockSourceDivider = TIMER_D_CLOCKSOURCE_DIVIDER_1;
    pwm1Param.clockingMode = TIMER_D_CLOCKINGMODE_EXTERNAL_CLOCK;
    pwm1Param.timerPeriod = 128;
    pwm1Param.combineCCRRegistersCombination = TIMER_D_COMBINE_CCR1_CCR2;
    pwm1Param.compareOutputMode = TIMER_D_OUTPUTMODE_RESET_SET;
    pwm1Param.dutyCycle1 = 40;
    pwm1Param.dutyCycle2 = 80;
    Timer_D_combineTDCCRToOutputPWM(TIMER_D0_BASE, &pwm1Param);

    // Configure TimerD1 to combine CCR0/1 block to control TD1CCR2 PWM output
    Timer_D_combineTDCCRToOutputPWMParam pwm2Param = {0};
    pwm2Param.clockSource = TIMER_D_CLOCKSOURCE_SMCLK;
    pwm2Param.clockSourceDivider = TIMER_D_CLOCKSOURCE_DIVIDER_1;
    pwm2Param.clockingMode = TIMER_D_CLOCKINGMODE_EXTERNAL_CLOCK;
    pwm2Param.timerPeriod = 128;
    pwm2Param.combineCCRRegistersCombination = TIMER_D_COMBINE_CCR1_CCR2;
    pwm2Param.compareOutputMode = TIMER_D_OUTPUTMODE_RESET_SET;
    pwm2Param.dutyCycle1 = 40;
    pwm2Param.dutyCycle2 = 96;
    Timer_D_combineTDCCRToOutputPWM(TIMER_D1_BASE, &pwm2Param);

    Timer_D_initUpModeParam initUp1Param = {0};
    initUp1Param.clockSource = TIMER_D_CLOCKSOURCE_ACLK;
    initUp1Param.clockSourceDivider = TIMER_D_CLOCKSOURCE_DIVIDER_1;
    initUp1Param.clockingMode = TIMER_D_CLOCKINGMODE_EXTERNAL_CLOCK;
    initUp1Param.timerPeriod = 128-1;
    initUp1Param.timerInterruptEnable_TDIE = TIMER_D_TDIE_INTERRUPT_DISABLE;
    initUp1Param.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initUp1Param.timerClear = TIMER_D_DO_CLEAR;
    Timer_D_initUpMode(TIMER_D0_BASE, &initUp1Param);

    Timer_D_initUpModeParam initUp2Param = {0};
    initUp2Param.clockSource = TIMER_D_CLOCKSOURCE_ACLK;
    initUp2Param.clockSourceDivider = TIMER_D_CLOCKSOURCE_DIVIDER_1;
    initUp2Param.clockingMode = TIMER_D_CLOCKINGMODE_EXTERNAL_CLOCK;
    initUp2Param.timerPeriod = 128-1;
    initUp2Param.timerInterruptEnable_TDIE = TIMER_D_TDIE_INTERRUPT_DISABLE;
    initUp2Param.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initUp2Param.timerClear = TIMER_D_DO_CLEAR;
    Timer_D_initUpMode(TIMER_D1_BASE, &initUp2Param);

    Timer_D_startCounter(TIMER_D0_BASE,
    	  TIMER_D_UP_MODE
    	  );
    Timer_D_startCounter(TIMER_D1_BASE,
    	  TIMER_D_UP_MODE
    		  );

    __bis_SR_register(LPM0_bits);             // Enter LPM0
    __no_operation();                         // For debugger

}

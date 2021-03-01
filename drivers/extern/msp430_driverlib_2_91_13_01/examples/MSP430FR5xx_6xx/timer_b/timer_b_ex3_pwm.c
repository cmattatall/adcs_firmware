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
//   Timer_B, PWM TB1-6, Up Mode, DCO SMCLK
//
//   Description: This program generates two PWM outputs on P2.6/P2.2 using
//   Timer_B configured for up mode. The value in CCR0, 512-1, defines the PWM
//   period and the values in CCR1-2 the PWM duty cycles. Using ~1048kHz SMCLK
//   as TBCLK, the timer period is ~488us.
//   ACLK = 32kHz, SMCLK = MCLK = TBCLK = default DCO ~1048kHz.
//
//   Tested On: MSP430FR5969
//             -----------------
//         /|\|              XIN|-
//          | |                 |  32kHz
//          --|RST          XOUT|-
//            |                 |
//            |       P2.6/TB0.1|--> CCR1 - 75% PWM
//            |       P2.2/TB0.2|--> CCR2 - 25% PWM
//            |                 |
//            |                 |
//            |                 |
//            |                 |
//
//******************************************************************************
#include "driverlib.h"

void main(void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_P2,
        GPIO_PIN6 | GPIO_PIN2,
        GPIO_PRIMARY_MODULE_FUNCTION
        );

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    //Start timer
	Timer_B_initUpModeParam param = {0};
    param.clockSource = TIMER_B_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1;
    param.timerPeriod = 511;
    param.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_DISABLE;
    param.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_B_CCIE_CCR0_INTERRUPT_DISABLE;
    param.timerClear = TIMER_B_DO_CLEAR;
    param.startTimer = true;
    Timer_B_initUpMode(TIMER_B0_BASE, &param);

    //Initialize compare mode to generate PWM1
   	Timer_B_initCompareModeParam param1 = {0};
    param1.compareRegister = TIMER_B_CAPTURECOMPARE_REGISTER_1;
    param1.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_DISABLE;
    param1.compareOutputMode = TIMER_B_OUTPUTMODE_RESET_SET;
    param1.compareValue = 383;
    Timer_B_initCompareMode(TIMER_B0_BASE, &param1);
	
	//Initialize compare mode to generate PWM2
	Timer_B_initCompareModeParam param2 = {0};
    param2.compareRegister = TIMER_B_CAPTURECOMPARE_REGISTER_2;
    param2.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_DISABLE;
    param2.compareOutputMode = TIMER_B_OUTPUTMODE_RESET_SET;
    param2.compareValue = 128;
    Timer_B_initCompareMode(TIMER_B0_BASE, &param2);

     _BIS_SR(LPM0_bits + GIE);                 // CPU off
}

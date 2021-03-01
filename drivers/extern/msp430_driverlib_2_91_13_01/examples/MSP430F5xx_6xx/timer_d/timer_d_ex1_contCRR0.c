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
//  MSP430F51x2 Demo - TimerD0, Toggle P1.0, CCR0 Cont. Mode ISR, DCO SMCLK
//
//  Description: Toggle P1.0 using software and TD_0 ISR. Toggles every
//  50000 SMCLK cycles. SMCLK provides clock source for TDCLK.
//  During the TD_0 ISR, P1.0 is toggled and 50000 clock cycles are added to
//  CCR0. TD_0 ISR is triggered every 50000 cycles. CPU is normally off and
//  used only during TD_ISR. Toggle rate = 50000/1.045Mhz ~ 48ms.
//  ACLK = n/a, MCLK = SMCLK = TDCLK = default DCO ~1.045MHz
//
//           MSP430F51x2
//         ---------------
//     /|\|               |
//      | |               |
//      --|RST            |
//        |               |
//        |           P1.0|-->LED
//
//******************************************************************************

#include "driverlib.h"

void main(void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    //Set P1.0 to output direction
    GPIO_setAsOutputPin(
	      GPIO_PORT_P1,
	      GPIO_PIN0
	      );

    //Start timer in continuous mode sourced by SMCLK
    Timer_D_initContinuousModeParam initContparam = {0};
    initContparam.clockSource = TIMER_D_CLOCKSOURCE_SMCLK;
    initContparam.clockSourceDivider = TIMER_D_CLOCKSOURCE_DIVIDER_1;
    initContparam.clockingMode = TIMER_D_CLOCKINGMODE_EXTERNAL_CLOCK;
    initContparam.timerInterruptEnable_TDIE = TIMER_D_TDIE_INTERRUPT_DISABLE;
    initContparam.timerClear = TIMER_D_DO_CLEAR;
    Timer_D_initContinuousMode(TIMER_D0_BASE, &initContparam);

    Timer_D_startCounter(TIMER_D0_BASE,
      TIMER_D_CONTINUOUS_MODE
		  );

    //Initiaze compare mode
    Timer_D_clearCaptureCompareInterrupt(TIMER_D0_BASE,
	      TIMER_D_CAPTURECOMPARE_REGISTER_0);


    Timer_D_initCompareModeParam initCompParam = {0};
    initCompParam.compareRegister = TIMER_D_CAPTURECOMPARE_REGISTER_0;
    initCompParam.compareInterruptEnable = TIMER_D_CAPTURECOMPARE_INTERRUPT_ENABLE;
    initCompParam.compareOutputMode = TIMER_D_OUTPUTMODE_OUTBITVALUE;
    initCompParam.compareValue = 50000;
    Timer_D_initCompareMode(TIMER_D0_BASE, &initCompParam);

    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, enable interrupts
    __no_operation();                         // For debugger
}

// Timer0_D0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_D0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER0_D0_VECTOR)))
#endif
void TIMER0_D0_ISR(void)
{
    //Toggle P1.0
    GPIO_toggleOutputOnPin(

      GPIO_PORT_P1,
      GPIO_PIN0
      );

    //Add Offset to CCR0
    Timer_D_setCompareValue(TIMER_D0_BASE,
      TIMER_D_CAPTURECOMPARE_REGISTER_0,
      5000
      );
}


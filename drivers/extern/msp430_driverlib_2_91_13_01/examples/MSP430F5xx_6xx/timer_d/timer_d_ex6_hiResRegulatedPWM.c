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
//  MSP430F51x2 Demo - TimerD0, Hi-Res Regulated Mode, PWM TD0.0-2, DCO SMCLK
//
//  Description: This code example shows how to configure TimerD in Hi-Res
//  regulated mode. The reference input clock freq (in this e.g. SMCLK=12MHz)
//  is multiplied by a factor of 16 to generate a hi-res freq of InputClk x 16
//  (in this e.g. 12MHz x 16 = 192Mhz. With TD0CCR0=128-1, TD0.0 outputs a
//  square wave of freq 746kHz and TD0.1 and TD0.2 output PWM waveforms with
//  dutycycles of 25% and 75% respectively.
//
//  NOTE: For using multiply factor 16x in hi-res calibration mode, make sure
//  input ref clock is in the following freq range: 8Mhz to 16MHz.
//
//  ACLK=LFXT1=32kHz; SMCLK=MCLK=12MHz; TDCLK(Hi-Res)= SMCLK x 16 = 192MHz
//
//                 MSP430F51x2
//             -----------------
//         /|\|              XIN|-
//          | |                 | 32kHz
//          --|RST          XOUT|-
//            |                 |
//            |       P1.6/TD0.0|--> CCR0 - 50% dutycycle; PWM period = (2 x 128)/(12MHz x 16)~ 1.34us = 746kHz
//            |       P1.7/TD0.1|--> CCR1 - 25% dutycycle; ON period ~ 32/(12MHz x 16)~ 167ns
//            |       P2.0/TD0.2|--> CCR2 - 75% dutycycle; ON period ~ 96/(12MHz x 16)~ 500ns
//
//******************************************************************************
#include "driverlib.h"

void main(void)
{
    WDT_A_hold(WDT_A_BASE);

    // Configure XT1
    // Port select XT1
    GPIO_setAsPeripheralModuleFunctionInputPin(
          		  GPIO_PORT_PJ,
          		  GPIO_PIN4 + GPIO_PIN5
          		  );

    UCS_turnOnLFXT1 (UCS_XT1_DRIVE_3,
    		  UCS_XCAP_3
    		  );

    // Increase Vcore setting to level1 to support fsystem=12MHz
    PMM_setVCore (PMMCOREV_1);

    // Initialize DCO to 12MHz
    UCS_initFLLSettle(12000,
                  374);

    // Configure TimerD in Hi-Res Regulated Mode
    Timer_D_initHighResGeneratorInRegulatedModeParam param = {0};
    param.clockSource = TIMER_D_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_D_CLOCKSOURCE_DIVIDER_1;
    param.clockingMode = TIMER_D_CLOCKINGMODE_HIRES_LOCAL_CLOCK;
    param.highResClockMultiplyFactor = TIMER_D_CLOCKSOURCE_DIVIDER_1;
    param.highResClockDivider = TIMER_D_HIGHRES_CLK_MULTIPLY_FACTOR_16x;
    Timer_D_initHighResGeneratorInRegulatedMode(TIMER_D0_BASE, &param);

    Timer_D_enableHighResInterrupt(TIMER_D0_BASE,
                  TIMER_D_HIGH_RES_FREQUENCY_LOCK
                  );

    // Configure TimerD in Hi-Res Regulated Mode
    // TDCLK=SMCLK=12MHz=Hi-Res input clk select
    // Configure the CCRx blocks
    // Configure the CCRx blocks
    Timer_D_initCompareModeParam initComp0Param = {0};
    initComp0Param.compareRegister = TIMER_D_CAPTURECOMPARE_REGISTER_0;
    initComp0Param.compareInterruptEnable = TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp0Param.compareOutputMode = TIMER_D_OUTPUTMODE_TOGGLE;
    initComp0Param.compareValue = 128-1;
    Timer_D_initCompareMode(TIMER_D0_BASE, &initComp0Param);

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

    Timer_D_initUpModeParam initUpParam = {0};
    initUpParam.clockSource = TIMER_D_CLOCKSOURCE_SMCLK;
    initUpParam.clockSourceDivider = TIMER_D_CLOCKSOURCE_DIVIDER_1;
    initUpParam.clockingMode = TIMER_D_CLOCKINGMODE_HIRES_LOCAL_CLOCK;
    initUpParam.timerPeriod = 128-1;
    initUpParam.timerInterruptEnable_TDIE = TIMER_D_TDIE_INTERRUPT_DISABLE;
    initUpParam.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initUpParam.timerClear = TIMER_D_DO_CLEAR;
    Timer_D_initUpMode(TIMER_D0_BASE, &initUpParam);

    Timer_D_startCounter(TIMER_D0_BASE,
      		 TIMER_D_UP_MODE
      		 );

    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0
    __no_operation();                         // For debugger
}

// Timer0_D1 Interrupt Vector (TDIV) handler
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_D1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER0_D1_VECTOR)))
#endif
void TIMER0_D1_ISR(void)
{
    switch(__even_in_range(TD0IV,30))
    {
    case  0: break;                          // No interrupt
    case  2: break;                          // CCR1 not used
    case  4: break;                          // CCR2 not used
    case  6: break;                          // reserved
    case  8: break;                          // reserved
    case 10: break;                          // reserved
    case 12: break;                          // reserved
    case 14: break;
    case 16: break;
    case 18:                                 // Clock fail low
      while(1);                              // Input ref clock freq too low; trap here
    case 20:                                 // Clock fail high
      while(1);                              // Input ref clock freq too high; trap here
    case 22:                                 // Hi-res freq locked
      // Hi-Res freq locked; now configure ports to output PWMs at TD0.0/1/2
      // P1.6,7 option select
      // P1.6,7 output
      // P2.0 options select
      // P2.0 output
      GPIO_setAsPeripheralModuleFunctionOutputPin(
          		  GPIO_PORT_P1,
          		  GPIO_PIN6 + GPIO_PIN7
          		  );

      GPIO_setAsPeripheralModuleFunctionOutputPin(
          		  GPIO_PORT_P2,
          		  GPIO_PIN0
          		  );
      break;
    case 24: break;                          // Hi-res freq unlocked
    case 26: break;                          // reserved
    case 28: break;                          // reserved
    case 30: break;                          // reserved
    default: break;
    }
}



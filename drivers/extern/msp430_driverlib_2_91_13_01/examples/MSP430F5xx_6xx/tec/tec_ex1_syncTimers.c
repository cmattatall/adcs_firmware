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
//  MSP430F51x2 Demo - TimerD0/1-TEC, Synchronizing timers, Normal timer mode
//
//  Description: This code example shows how to configure the TEC module for
//  synchronizing two timer instances (TD0 and TD1). In this example code, TD0 is
//  the master timer and TD1 is the slave instance. The clock configuration of
//  the master is applied to the slave. The counter length, high res enable
//  and timer mode settings of the master are applied to the slave as well.
//
//  Note: TDxCCR0 registers of both master and slave timer instance (TD0 and
//      TD1 in this case) should be the same value
//
//  ACLK = REFO = 32kHz; SMCLK = MCLK = DCO Clock = 12MHz; TD0 = TD1 = 10kHz
//
//                 MSP430F51x2
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |       P1.6/TD0.0|--> CCR0 - 50% dutycycle
//            |       P2.1/TD1.0|--> CCR0 - 50% dutycycle
//            |                 |
//            |       P1.7/TD0.1|--> CCR1 - 20% dutycycle
//            |       P2.2/TD1.1|--> CCR1 - 60% dutycycle
//            |                 |
//            |       P2.0/TD0.2|--> CCR2 - 40% dutycycle
//            |       P2.3/TD1.2|--> CCR2 - 80S% dutycycle
//
//******************************************************************************
#include "driverlib.h"

//*****************************************************************************
//
//Target frequency for MCLK in kHz
//
//*****************************************************************************
#define UCS_MCLK_DESIRED_FREQUENCY_IN_KHZ   24000

//*****************************************************************************
//
//MCLK/FLLRef Ratio
//
//*****************************************************************************
#define UCS_MCLK_FLLREF_RATIO   374

void main(void)
{

    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    // Increase Vcore setting to level1 to support fsystem=12MHz
    // NOTE: Change core voltage one level at a time..
    PMM_setVCore(PMMCOREV_1);

    // Set DCO FLL reference = REFO
    UCS_initClockSignal(UCS_FLLREF,
      UCS_REFOCLK_SELECT,
      UCS_CLOCK_DIVIDER_1
    );

    // Set ACLK = REFO
    UCS_initClockSignal(UCS_ACLK,
      UCS_REFOCLK_SELECT,
      UCS_CLOCK_DIVIDER_1
    );

    // (N + 1) * FLLRef = Fdco
    // (374 + 1) * 32768 = 12MHz
    // Set FLL Div = fDCOCLK/2
    // Enable the FLL control loop
    UCS_initFLLSettle(UCS_MCLK_DESIRED_FREQUENCY_IN_KHZ,
      UCS_MCLK_FLLREF_RATIO
    );

    // P1.6/TD0.0, P1.7,TD0.1, options select
    // Output direction
    GPIO_setAsPeripheralModuleFunctionOutputPin(
      GPIO_PORT_P1,
      GPIO_PIN6 | GPIO_PIN7
      );

    // P2.0/TD0.2, P2.1/TD1.0, P2.2/TD1.1, P2.3/TD1.2, options select
    // Output direction
    GPIO_setAsPeripheralModuleFunctionOutputPin(
      GPIO_PORT_P2,
      GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3
      );


    // Configure Master Timer Instance - TimerD0
    // Freq = 100kHz, TD0.1/TD0.2 PWM Period = 10us
    Timer_D_initUpModeParam initUpParam = {0};
    initUpParam.clockSource = TIMER_D_CLOCKSOURCE_SMCLK;
    initUpParam.clockSourceDivider = TIMER_D_CLOCKSOURCE_DIVIDER_1;
    initUpParam.clockingMode = TIMER_D_CLOCKINGMODE_EXTERNAL_CLOCK;
    initUpParam.timerPeriod = 120;
    initUpParam.timerInterruptEnable_TDIE = TIMER_D_TDIE_INTERRUPT_DISABLE;
    initUpParam.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initUpParam.timerClear = TIMER_D_DO_CLEAR;
    Timer_D_initUpMode(TIMER_D0_BASE, &initUpParam);

    Timer_D_initCompareModeParam initComp0Param = {0};
    initComp0Param.compareRegister = TIMER_D_CAPTURECOMPARE_REGISTER_0;
    initComp0Param.compareInterruptEnable = TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp0Param.compareOutputMode = TIMER_D_OUTPUTMODE_TOGGLE;
    initComp0Param.compareValue = 120;
    Timer_D_initCompareMode(TIMER_D0_BASE, &initComp0Param);

    Timer_D_initCompareModeParam initComp1Param = {0};
    initComp1Param.compareRegister = TIMER_D_CAPTURECOMPARE_REGISTER_1;
    initComp1Param.compareInterruptEnable = TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp1Param.compareOutputMode = TIMER_D_OUTPUTMODE_RESET_SET;
    initComp1Param.compareValue = 24;
    Timer_D_initCompareMode(TIMER_D0_BASE, &initComp1Param);

    Timer_D_initCompareModeParam initComp2Param = {0};
    initComp2Param.compareRegister = TIMER_D_CAPTURECOMPARE_REGISTER_2;
    initComp2Param.compareInterruptEnable = TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp2Param.compareOutputMode = TIMER_D_OUTPUTMODE_RESET_SET;
    initComp2Param.compareValue = 48;
    Timer_D_initCompareMode(TIMER_D0_BASE, &initComp2Param);

    // Configure Slave Timer Instance - TimerD1 PWM outputs
    Timer_D_initUpModeParam initUpParam1 = {0};
    initUpParam1.clockSource = TIMER_D_CLOCKSOURCE_EXTERNAL_TDCLK;
    initUpParam1.clockSourceDivider = TIMER_D_CLOCKSOURCE_DIVIDER_1;
    initUpParam1.clockingMode = TIMER_D_CLOCKINGMODE_AUXILIARY_CLK;
    initUpParam1.timerPeriod = 120;
    initUpParam1.timerInterruptEnable_TDIE = TIMER_D_TDIE_INTERRUPT_DISABLE;
    initUpParam1.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initUpParam1.timerClear = TIMER_D_DO_CLEAR;
    Timer_D_initUpMode(TIMER_D1_BASE, &initUpParam1);

    Timer_D_initCompareModeParam initComp0Param1 = {0};
    initComp0Param1.compareRegister = TIMER_D_CAPTURECOMPARE_REGISTER_0;
    initComp0Param1.compareInterruptEnable = TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp0Param1.compareOutputMode = TIMER_D_OUTPUTMODE_TOGGLE;
    initComp0Param1.compareValue = 120;
    Timer_D_initCompareMode(TIMER_D1_BASE, &initComp0Param1);

    Timer_D_initCompareModeParam initComp1Param1 = {0};
    initComp1Param1.compareRegister = TIMER_D_CAPTURECOMPARE_REGISTER_1;
    initComp1Param1.compareInterruptEnable = TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp1Param1.compareOutputMode = TIMER_D_OUTPUTMODE_RESET_SET;
    initComp1Param1.compareValue = 72;
    Timer_D_initCompareMode(TIMER_D1_BASE, &initComp1Param1);

    Timer_D_initCompareModeParam initComp2Param1 = {0};
    initComp2Param1.compareRegister = TIMER_D_CAPTURECOMPARE_REGISTER_2;
    initComp2Param1.compareInterruptEnable = TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp2Param1.compareOutputMode = TIMER_D_OUTPUTMODE_RESET_SET;
    initComp2Param1.compareValue = 96;
    Timer_D_initCompareMode(TIMER_D1_BASE, &initComp2Param1);

    // Syncronizing master (TD0) and slave (TD1) timer instances
    // Enable synchronized clear by enabling Aux clear of slave timer
    TEC_enableAuxiliaryClearSignal(TEC1_BASE);

    // Clear timer counter, Up mode, Start timers
    Timer_D_startCounter(TIMER_D0_BASE,
      TIMER_D_UP_MODE);

    __bis_SR_register(LPM0_bits);             // Enter LPM0
    __no_operation();                         // For debugger

}

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
//  MSP430F51x2 Demo - TimerD0/1-TEC, External Clear, Level trigger
//
//  Description: This code example shows how to configure the TEC module for
//  controlling the timerD PWM dutycycle from external clear input. In this
//  example code, TD0 is used to generate the external clear pulses. TD1 is
//  configired to output PWM of 25%, 50% and 75%. With the EXTCLR clear event
//  on TD1 input, the TD1 timer counter (TDR) gets reset and this modifies the
//  dutycycle of all the TD1 PWM channels (TD1.0, TD1.1, TD1.2)
//  TD0 = 1KHz, 15% dutycycle; TD1 = 10kHz, 25%,50%,75% dutycycle
//
//  Note: EXTCLR event in this case is High level on EXTCLR input. As long as,
//       the input on EXTCLR is high, the timer counter will be held in reset
//
//  ACLK = REFO = 32kHz; SMCLK = MCLK = DCO Clock = 12MHz;
//
//                 MSP430F51x2
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |       P2.0/TD0.2|--> CCR2 -| ~15% duty cycle; External Clear Pulses
//            |                 |          |
//            |     P2.7/TEC1CLR|<---------| External Clear Input
//            |                 |
//            |       P2.1/TD1.0|--> CCR0 - 50% dutycycle (Period/2)
//            |       P2.2/TD1.1|--> CCR1 - 25% dutycycle
//            |       P2.3/TD1.2|--> CCR2 - 75% dutycycle
//
//  B. Nisarga
//  Texas Instruments Inc.
//  Nov 2010
//  Built with CCS v4 and IAR Embedded Workbench Version: 4.21
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

    // Configure DCO = 12Mhz
    UCS_initClockSignal(UCS_FLLREF,
        UCS_REFOCLK_SELECT,
        UCS_CLOCK_DIVIDER_1
    );

    UCS_initClockSignal(UCS_ACLK,
        UCS_REFOCLK_SELECT,
        UCS_CLOCK_DIVIDER_1
    );

    UCS_initFLLSettle(UCS_MCLK_DESIRED_FREQUENCY_IN_KHZ,
        UCS_MCLK_FLLREF_RATIO
    );

    // Configure TD0 port pins
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_P2,
        GPIO_PIN0
    );

    // Configure TimerD0 to combine CCR0/1 block to control TD0CCR2 PWM output
    // TD0CCR2 PWM output = External clear pulses for TD1
    // Freq = 1kHz, Period = 1ms; dutycycle = 15% => ON period = 150us
    Timer_D_combineTDCCRToOutputPWMParam outputPWMParam = {0};
    outputPWMParam.clockSource = TIMER_D_CLOCKSOURCE_ACLK;
    outputPWMParam.clockSourceDivider = TIMER_D_CLOCKSOURCE_DIVIDER_1;
    outputPWMParam.clockingMode = TIMER_D_CLOCKINGMODE_EXTERNAL_CLOCK;
    outputPWMParam.timerPeriod = 32;
    outputPWMParam.combineCCRRegistersCombination = TIMER_D_COMBINE_CCR1_CCR2;
    outputPWMParam.compareOutputMode = TIMER_D_OUTPUTMODE_RESET_SET;
    outputPWMParam.dutyCycle1 = 20;
    outputPWMParam.dutyCycle2 = 25;
    Timer_D_combineTDCCRToOutputPWM(TIMER_D0_BASE, &outputPWMParam);

    Timer_D_startCounter(TIMER_D0_BASE,
        TIMER_D_UP_MODE);

    // Configure TD1 port pins
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_P2,
        GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3
    );


    // Configure TimerD1 to 10kHz with PWMs dutycycles = 25%, 50%, 75%
    Timer_D_initUpModeParam initUpParam = {0};
    initUpParam.clockSource = TIMER_D_CLOCKSOURCE_SMCLK;
    initUpParam.clockSourceDivider = TIMER_D_CLOCKSOURCE_DIVIDER_1;
    initUpParam.clockingMode = TIMER_D_CLOCKINGMODE_EXTERNAL_CLOCK;
    initUpParam.timerPeriod = 1200;
    initUpParam.timerInterruptEnable_TDIE = TIMER_D_TDIE_INTERRUPT_DISABLE;
    initUpParam.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initUpParam.timerClear = TIMER_D_DO_CLEAR;
    Timer_D_initUpMode(TIMER_D1_BASE, &initUpParam);

    Timer_D_initCompareModeParam initComp0Param = {0};
    initComp0Param.compareRegister = TIMER_D_CAPTURECOMPARE_REGISTER_0;
    initComp0Param.compareInterruptEnable = TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp0Param.compareOutputMode = TIMER_D_OUTPUTMODE_TOGGLE;
    initComp0Param.compareValue = 1200;
    Timer_D_initCompareMode(TIMER_D1_BASE, &initComp0Param);

    Timer_D_initCompareModeParam initComp1Param = {0};
    initComp1Param.compareRegister = TIMER_D_CAPTURECOMPARE_REGISTER_1;
    initComp1Param.compareInterruptEnable = TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp1Param.compareOutputMode = TIMER_D_OUTPUTMODE_RESET_SET;
    initComp1Param.compareValue = 300;
    Timer_D_initCompareMode(TIMER_D1_BASE, &initComp1Param);

    Timer_D_initCompareModeParam initComp2Param = {0};
    initComp2Param.compareRegister = TIMER_D_CAPTURECOMPARE_REGISTER_2;
    initComp2Param.compareInterruptEnable = TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp2Param.compareOutputMode = TIMER_D_OUTPUTMODE_RESET_SET;
    initComp2Param.compareValue = 900;
    Timer_D_initCompareMode(TIMER_D1_BASE, &initComp2Param);

    Timer_D_startCounter(TIMER_D1_BASE,
        TIMER_D_UP_MODE);

    // Configure TD1 TEC External Clear
    // Need to physically connect P2.0/TD0.2 to P2.7/TEC1CLR
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P2,
        GPIO_PIN7
    );

    // High Level trigger, ext clear enable
    TEC_initExternalClearInput(TEC1_BASE,
    							 TEC_EXTERNAL_CLEAR_SIGNALTYPE_LEVEL_SENSITIVE,
    							 TEC_EXTERNAL_CLEAR_SIGNAL_NOT_HELD,
    							 TEC_EXTERNAL_CLEAR_POLARITY_RISING_EDGE_OR_HIGH_LEVEL
    							 );
    TEC_enableExternalClearInput(TEC1_BASE);


    __bis_SR_register(LPM0_bits);             // Enter LPM0
    __no_operation();                         // For debugger

}




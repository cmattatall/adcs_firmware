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
//  MSP430F51x2 Demo - TimerD0, Toggle P2.3/TD0.0, Up/Down Mode, 32kHz ACLK
//
//  Description: Toggle P1.6 using hardware TD0.0 output. TimerD0 is configured
//  for up/down mode with CCR0 defining period, TD0.0 also output on P1.6. In
//  this example, CCR0 is loaded with 5 and TD0.0 will toggle P2.3 at TDCLK/2*5.
//  Thus the output frequency on P1.6 will be the TDCLK/20. No CPU or software
//  resources required. Normal operating mode is LPM3.
//  As coded with TDCLK = ACLK, P2.3 output frequency = 32768/20 = 1.6384kHz
//  ACLK = TDCLK = LFXT1 = 32kHz, MCLK = default DCO ~1.045MHz
//
//               MSP430F51x2
//            -------------------
//        /|\ |              XIN|-
//         |  |                 | 32kHz
//         ---|RST          XOUT|-
//           |                   |
//           |         P1.6/TD0.0|-->ACLK/20
//
//******************************************************************************
#include "driverlib.h"

void main(void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    // Configure ports
    GPIO_setAsPeripheralModuleFunctionOutputPin(
    	  GPIO_PORT_P1,
    	  GPIO_PIN6
    	  );

    // XT1 configure
    GPIO_setAsPeripheralModuleFunctionInputPin(
    	  GPIO_PORT_PJ,
    	   GPIO_PIN4 + GPIO_PIN5
    	  );

    UCS_initClockSignal(UCS_FLLREF,
    		  UCS_XT1CLK_SELECT,
    		  UCS_CLOCK_DIVIDER_1
    		  );

    UCS_turnOnLFXT1 (UCS_XT1_DRIVE_3,
    	  UCS_XCAP_3
      );

    UCS_initClockSignal(UCS_ACLK,
    		  UCS_XT1CLK_SELECT,
    		  UCS_CLOCK_DIVIDER_1
    		  );

    Timer_D_initCompareModeParam initCompParam = {0};
    initCompParam.compareRegister = TIMER_D_CAPTURECOMPARE_REGISTER_0;
    initCompParam.compareInterruptEnable = TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initCompParam.compareOutputMode = TIMER_D_OUTPUTMODE_TOGGLE;
    initCompParam.compareValue = 5;
    Timer_D_initCompareMode(TIMER_D0_BASE, &initCompParam);

    Timer_D_initUpDownModeParam initUpDownParam = {0};
    initUpDownParam.clockSource = TIMER_D_CLOCKSOURCE_ACLK;
    initUpDownParam.clockSourceDivider = TIMER_D_CLOCKSOURCE_DIVIDER_1;
    initUpDownParam.clockingMode = TIMER_D_CLOCKINGMODE_EXTERNAL_CLOCK;
    initUpDownParam.timerPeriod = 5;
    initUpDownParam.timerInterruptEnable_TDIE = TIMER_D_TDIE_INTERRUPT_DISABLE;
    initUpDownParam.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initUpDownParam.timerClear = TIMER_D_DO_CLEAR;
    Timer_D_initUpDownMode(TIMER_D0_BASE, &initUpDownParam);

    Timer_D_startCounter(
    		  TIMER_D0_BASE,
    		  TIMER_D_UPDOWN_MODE
    		  );

    __bis_SR_register(LPM3_bits);             // Enter LPM3
    __no_operation();                         // For debugger
}

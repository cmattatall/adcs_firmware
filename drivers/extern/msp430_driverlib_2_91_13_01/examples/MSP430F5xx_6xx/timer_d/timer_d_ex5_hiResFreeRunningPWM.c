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
//  MSP430F51x2 Demo - TimerD0, Hi-Res Free Running mode, PWM TD0.0-2, TDCLK = 64Mhz
//
//  Description: This code example shows how to configure TimerD in Hi-Res
//  Free Running mode. The TD0HCTL1 register with the TDHCLKTRIMx, TDHCLKSRx and
//  TDHCLKRx bit settings are configured with the calibration data in the TLV
//  table.
//
//  SMCLK = MCLK = DCOclock = default DCO; TDCLK(TLV Cal Data)= 64Mhz
//  Note: 1. TDHMx=00(8x) should be used to use the 64Mhz TimerD TLV calibration data
//        2. Use code example with hal_tlv.c and hal_tlv.h files to read the TimerD TLV data
//
//                 MSP430F51x2
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |       P1.6/TD0.0|--> CCR0 - 50% dutycycle; PWM period = (2 x 128)/64MHz~ 4us = 250kHz
//            |       P1.7/TD0.1|--> CCR1 - 25% dutycycle; ON period ~ 32/64MHz~ 500ns
//            |       P2.0/TD0.2|--> CCR2 - 75% dutycycle; ON period ~ 96/64MHz~ 1500ns
//
//******************************************************************************
#include "driverlib.h"

void main(void)
{
    struct s_TLV_Timer_D_Cal_Data * pTD0CAL;  // Structure initialized in tlv.h
    uint8_t bTD0CAL_bytes;

    // Stop WDT
    WDT_A_hold(WDT_A_BASE);

    //Get TimerD0 Cal Values (instance 0)
    TLV_getInfo(TLV_TAG_TIMER_D_CAL, 0,
    	  &bTD0CAL_bytes,
    	  (uint16_t **) &pTD0CAL
    	  );

    if(bTD0CAL_bytes == 0x0)
    {
      // No TimerD free running cal data found
      while(1);                             // Loop here
    }

    // Configure TimerD in Hi-Res Free Running Mode
    Timer_D_initHighResGeneratorInFreeRunningMode(TIMER_D0_BASE,
    	  TIMER_D_HIGHRES_64MHZ
    	  );

    // Configure TD0.x GPIO pins
    GPIO_setAsPeripheralModuleFunctionOutputPin(
    		  GPIO_PORT_P1,
    		  GPIO_PIN6 + GPIO_PIN7
    		  );

    GPIO_setAsPeripheralModuleFunctionOutputPin(
    		  GPIO_PORT_P2,
    		  GPIO_PIN0
    		  );

    // Configure the CCRx blocks
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

    Timer_D_initCompareModeParam initComp3Param = {0};
    initComp3Param.compareRegister = TIMER_D_CAPTURECOMPARE_REGISTER_0;
    initComp3Param.compareInterruptEnable = TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp3Param.compareOutputMode = TIMER_D_OUTPUTMODE_TOGGLE;
    initComp3Param.compareValue = 128-1;
    Timer_D_initCompareMode(TIMER_D0_BASE, &initComp3Param);

    Timer_D_initUpDownModeParam initUpDownParam = {0};
    initUpDownParam.clockSource = TIMER_D_CLOCKSOURCE_EXTERNAL_TDCLK;
    initUpDownParam.clockSourceDivider = TIMER_D_CLOCKSOURCE_DIVIDER_1;
    initUpDownParam.clockingMode = TIMER_D_CLOCKINGMODE_HIRES_LOCAL_CLOCK;
    initUpDownParam.timerPeriod = 128-1;
    initUpDownParam.timerInterruptEnable_TDIE = TIMER_D_TDIE_INTERRUPT_DISABLE;
    initUpDownParam.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initUpDownParam.timerClear = TIMER_D_DO_CLEAR;
    Timer_D_initUpDownMode(TIMER_D0_BASE, &initUpDownParam);

    Timer_D_startCounter(TIMER_D0_BASE,
    	 TIMER_D_UP_MODE
    	 );

    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0
    __no_operation();                         // For debugger
}


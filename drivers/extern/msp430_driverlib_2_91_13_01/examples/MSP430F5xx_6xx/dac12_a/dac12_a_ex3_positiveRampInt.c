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
//! DAC12_A - Positive Ramp on DAC12_A_0
//!
//!  Using DAC12_A_0 and AVcc reference with a gain of 1, output positive ramp on
//!  DAC0. Normal mode is LPM0 with CPU off. Timer B used to provide a rising
//!  edge to Timer_B OUT2 signal to latch data from DAC0 data buffer. Timer_B
//!  OUT2 signal rises after the first ~31us, and then toggles for every ~31us
//!  afterwards, giving a period of ~62us for the DAC output to rise.
//!  ACLK = n/a, MCLK = SMCLK = default DCO
//!
//!                MSP430F66x
//!            -----------------
//!        /|\|              XIN|-
//!         | |                 |
//!         --|RST          XOUT|-
//!           |                 |
//!           |        P7.6/DAC0|-> Ramp_positive [Period ~400ms]
//!           |                 |
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - DAC12_A perpheral
//! - GPIO Port peripheral
//! - DAC0
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - DAC12_A_VECTOR
//!
//******************************************************************************

#include "driverlib.h"

static uint16_t dataToConvert = 0;

void main (void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);

    /*
     * // Initialize DAC12_A_0,
     * // Choose P7.6 as output
     * // Use AVcc as Vref+
     * // Multiply Vout by 1
     * // Use medium settling speed/current
     * // Use Timer_B OUT2 as data-latch trigger
     */
    DAC12_A_initParam param = {0};
    param.submoduleSelect = DAC12_A_SUBMODULE_0;
    param.outputSelect = DAC12_A_OUTPUT_1;
    param.positiveReferenceVoltage = DAC12_A_VREF_AVCC;
    param.outputVoltageMultiplier = DAC12_A_VREFx1;
    param.amplifierSetting = DAC12_A_AMP_MEDIN_MEDOUT;
    param.conversionTriggerSelect = DAC12_A_TRIGGER_TB;
    DAC12_A_init(DAC12_A_BASE, &param);

    //Enable the DAC12_A_0 interrupt
    DAC12_A_enableInterrupt(DAC12_A_BASE,
        DAC12_A_SUBMODULE_0);

    //Calibrate output buffer for DAC12_A_0
    DAC12_A_calibrateOutput(DAC12_A_BASE,
        DAC12_A_SUBMODULE_0);

    //Enable conversions for DAC12_A_0
    DAC12_A_enableConversions(DAC12_A_BASE,
        DAC12_A_SUBMODULE_0);

    //// Set the variable dataToConvert into data buffer for DAC12_A_0
    DAC12_A_setData(DAC12_A_BASE,
        DAC12_A_SUBMODULE_0,
        dataToConvert
        );

    //Start Timer B0
    Timer_B_initUpModeParam initUpModeParam = {0};
    initUpModeParam.clockSource = TIMER_B_CLOCKSOURCE_SMCLK;
    initUpModeParam.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1;
    initUpModeParam.timerPeriod = 0x33;
    initUpModeParam.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_DISABLE;
    initUpModeParam.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_B_CCIE_CCR0_INTERRUPT_DISABLE;
    initUpModeParam.timerClear = TIMER_B_DO_CLEAR;
    initUpModeParam.startTimer = true;
    Timer_B_initUpMode(TIMER_B0_BASE, &initUpModeParam);

   
	Timer_B_initCompareModeParam initCompParam = {0};
    initCompParam.compareRegister = TIMER_B_CAPTURECOMPARE_REGISTER_2;
    initCompParam.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initCompParam.compareOutputMode = TIMER_B_OUTPUTMODE_TOGGLE;
    initCompParam.compareValue = 0x32;
    Timer_B_initCompareMode(TIMER_B0_BASE, &initCompParam);

    //Enter LPM0
    __bis_SR_register(CPUOFF + GIE);
}

//******************************************************************************
//
//This is the DAC12_A_VECTOR interrupt vector service routine.
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=DAC12_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(DAC12_VECTOR)))
#endif
void DAC12_A_ISR (void)
{
    //Clear interrupt flag to stop asserts
    DAC12_A_clearInterrupt(DAC12_A_BASE,
        DAC12_A_SUBMODULE_0);

    //Increment ramp
    dataToConvert++;

    //Set the variable dataToConvert into data buffer for DAC12_A_0
    DAC12_A_setData(DAC12_A_BASE,
        DAC12_A_SUBMODULE_0,
        dataToConvert
        );
    __no_operation();
}

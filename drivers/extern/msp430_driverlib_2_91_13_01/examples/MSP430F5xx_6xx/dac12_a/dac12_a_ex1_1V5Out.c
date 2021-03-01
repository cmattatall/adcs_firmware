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
#include "driverlib.h"

//*******************************************************************************
//!
//! MSP430F6638 Example
//! DAC12 - 1.5V Output on DAC12_0
//!
//!  Using DAC12_0 and AVcc reference with a gain of 1, output 1.5V on DAC0.
//!  ACLK = n/a, MCLK = SMCLK = default DCO
//!
//!                MSP430F66x
//!            -----------------
//!        /|\|              XIN|-
//!         | |                 |
//!         --|RST          XOUT|-
//!           |                 |
//!           |        P7.6/DAC0|-> 1.5V
//!           |                 |
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - DAC12 peripheral
//! - GPIO Port peripheral
//! - DAC0
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - None.
//!
//******************************************************************************

void main (void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);

    /*
     * // Initialize DAC12_0,
     * // Choose P7.6 as output
     * // Use AVcc as Vref+
     * // Multiply Vout by 1
     * // Use medium settling speed/current
     * // Auto trigger as soon as data is set
     */
    DAC12_A_initParam param = {0};
    param.submoduleSelect = DAC12_A_SUBMODULE_0;
    param.outputSelect = DAC12_A_OUTPUT_1;
    param.positiveReferenceVoltage = DAC12_A_VREF_AVCC;
    param.outputVoltageMultiplier = DAC12_A_VREFx1;
    param.amplifierSetting = DAC12_A_AMP_MEDIN_MEDOUT;
    param.conversionTriggerSelect = DAC12_A_TRIGGER_ENCBYPASS;
    DAC12_A_init(DAC12_A_BASE, &param);

    //Calibrate output buffer for DAC12_0
    DAC12_A_calibrateOutput(DAC12_A_BASE,
        DAC12_A_SUBMODULE_0);

    //Set 0x07FF (~1.5V) into data buffer for DAC12_0
    DAC12_A_setData(DAC12_A_BASE,
        DAC12_A_SUBMODULE_0,
        0x7FF);

    //Enter LPM4
    __bis_SR_register(LPM4_bits);
}


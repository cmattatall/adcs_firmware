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
//! DAC12_A - 0.75V Output on DAC12_A
//!
//!  MSP430F6638 Example
//!  Using DAC12_A_1 and 1.5V reference with a gain of 1, output 0.75V on DAC1.
//!  ACLK = n/a, MCLK = SMCLK = default DCO
//!
//!                MSP430F66x
//!            -----------------
//!        /|\|              XIN|-
//!         | |                 |
//!         --|RST          XOUT|-
//!           |                 |
//!           |        P7.7/DAC1|-> 0.75V
//!           |                 |
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - DAC12_A peripheral
//! - GPIO Port peripheral
//! - DAC1
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - None.
//!
//******************************************************************************

#include "driverlib.h"

void main (void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);

    //Refernce is autmatically turned on from the REF module on DAC request
    //REF module default 1.5V is used

    /*
     * // Initialize DAC12_A_1,
     * // Choose P7.7 as output,
     * // Use internally generated 1.5V as Vref+,
     * // Multiply Vout by 1,
     * // Use medium settling speed/current,
     * // Auto trigger as soon as data is set
     */
    DAC12_A_initParam param = {0};
    param.submoduleSelect = DAC12_A_SUBMODULE_1;
    param.outputSelect = DAC12_A_OUTPUT_1;
    param.positiveReferenceVoltage = DAC12_A_VREF_INT;
    param.outputVoltageMultiplier = DAC12_A_VREFx1;
    param.amplifierSetting = DAC12_A_AMP_MEDIN_MEDOUT;
    param.conversionTriggerSelect = DAC12_A_TRIGGER_ENCBYPASS;
    DAC12_A_init(DAC12_A_BASE, &param);

    //Calibrate output buffer for DAC12_A_0
    DAC12_A_calibrateOutput(DAC12_A_BASE,
        DAC12_A_SUBMODULE_0);

    //Configure internal reference  If ref generator busy, WAIT
    while ( REF_ACTIVE == Ref_isRefGenBusy(REF_BASE) ) ;

    //Select internal ref = 1.5V
    Ref_setReferenceVoltage(REF_BASE,
        REF_VREF1_5V);

    //Internal Reference ON
    Ref_enableReferenceVoltage(REF_BASE);

    //Delay (~75us) for Ref to settle
    __delay_cycles(75);

    //Set 0x7FF (~0.75V) into data buffer for DAC12_A_1
    DAC12_A_setData(DAC12_A_BASE,
        DAC12_A_SUBMODULE_1,
        0x7FF
        );

    //Enter LPM4
    __bis_SR_register(LPM4_bits);
}


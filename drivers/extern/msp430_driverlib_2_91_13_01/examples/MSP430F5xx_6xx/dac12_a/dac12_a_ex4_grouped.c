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
//MSP430F66x Demo - DAC12_A_0 & DAC12_A_1 grouped, positive ramp on DAC12_A_0 and
//negative ramp on DAC12_A_1, high settling speed/current.
//!
//! \addtogroup dac12_examples_list
//! DAC12_A - Grouped, Positive Ramp on DAC12_A_0, Negative Ramp on DAC12_A_1,
//!       High Settling Speed/Current
//!
//!  Using DAC12_A_0 grouped with DAC12_A_1 and AVcc reference with a gain of 1,
//!  output positive ramp on DAC0, negative ramp on DAC1. Data is not latched
//!  from either data buffers until both data buffers have been set.
//!  ACLK = n/a, MCLK = SMCLK = default DCO
//!
//!                MSP430F66x
//!            -----------------
//!        /|\|              XIN|-
//!         | |                 |
//!         --|RST          XOUT|-
//!           |                 |
//!           |        P7.6/DAC0|-> Ramp_positive
//!           |        P7.7/DAC1|-> Ramp_negative
//!           |                 |
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - DAC12_A peripheral
//! - GPIO Port peripheral
//! - DAC0
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
    uint16_t dataToConvert = 0;

    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);

    /*
     * Initialize DAC12_A_0
     * Multiply Vout by 1
     * Use high settling speed/current,
     * Auto trigger when data is set as,
     * long as conversions are enabled
     */
	DAC12_A_initParam param0 = {0};
	param0.submoduleSelect = DAC12_A_SUBMODULE_0;
    param0.outputSelect = DAC12_A_OUTPUT_1;
    param0.positiveReferenceVoltage = DAC12_A_VREF_AVCC;
    param0.outputVoltageMultiplier = DAC12_A_VREFx1;
    param0.amplifierSetting = DAC12_A_AMP_HIGHIN_HIGHOUT;
    param0.conversionTriggerSelect = DAC12_A_TRIGGER_ENC;
	DAC12_A_init(DAC12_A_BASE, &param0);

    /*Initialize DAC12_A_1
     * Choose P6.7 as output
     * Use AVcc as Vref+
     * Multiply Vout by 1
     * Use high settling speed/current
     * Auto trigger when data is set as long as conversions are enabled
     */
    DAC12_A_initParam param1 = {0};
	param1.submoduleSelect = DAC12_A_SUBMODULE_1;
    param1.outputSelect = DAC12_A_OUTPUT_1;
    param1.positiveReferenceVoltage = DAC12_A_VREF_AVCC;
    param1.outputVoltageMultiplier = DAC12_A_VREFx1;
    param1.amplifierSetting = DAC12_A_AMP_HIGHIN_HIGHOUT;
    param1.conversionTriggerSelect = DAC12_A_TRIGGER_ENC;
	DAC12_A_init(DAC12_A_BASE, &param1);


    //Group both DAC12_A's to sync output
    DAC12_A_enableGrouping(DAC12_A_BASE);

    //Calibrate output buffer for DAC12_A_0
    DAC12_A_calibrateOutput(DAC12_A_BASE,
        DAC12_A_SUBMODULE_0);

    //Calibrate output buffer for DAC12_A_0
    DAC12_A_calibrateOutput(DAC12_A_BASE,
        DAC12_A_SUBMODULE_1);

    //Enable Conversions for both DAC12_A_0 and DAC12_A_1
    DAC12_A_enableConversions(DAC12_A_BASE,
        DAC12_A_SUBMODULE_0);
    DAC12_A_enableConversions(DAC12_A_BASE,
        DAC12_A_SUBMODULE_1);

    for (;;)
    {
        dataToConvert++;

        //Set data for DAC12_A_0, which will not latch because it is grouped
        DAC12_A_setData(DAC12_A_BASE,
            DAC12_A_SUBMODULE_0,
            dataToConvert);

        //Set data for DAC12_A_1, which will latch both DAC's data buffers
        DAC12_A_setData(DAC12_A_BASE,
            DAC12_A_SUBMODULE_1,
            ~(dataToConvert));
        __no_operation();
    }
}


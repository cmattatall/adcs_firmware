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
//!
//! ADC12_A - Sample A0 Input, External Ref
//!  MSP430F552x Demo
//!
//!  This example shows how to use an external positive reference for the ADC12_A.
//!  The external reference is applied to the VeREF+ pin. AVss is used for the
//!  negative reference. A single conversion is performed on channel A0. The
//!  conversion results are stored in ADC12MEM0 and Test by applying a voltage
//!  to channel A0, then setting and running to a break point at the "_NOP()"
//!  instruction. To view the conversion results, open an SFR window in debugger
//!  and view the contents of ADC12MEM0.
//!
//!  NOTE: VeREF+ range: 1.4V (min) to AVCC (max)
//!        VeREF- range: 0V (min) to 1.2V (max)
//!        Differential ref voltage range: 1.4V(min) to AVCC(max)
//!			(see datasheet for device specific information)
//!
//!                 MSP430F552x
//!             -------------------
//!         /|\|                   |
//!          | |                   |
//!          --|RST         P6.0/A0|<- Vin
//!            |        P5.0/VeREF+|<- REF
//!            |                   |
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - ADC12_A peripheral
//! - GPIO Port peripheral
//! - A0
//! - VeREF+
//!
//******************************************************************************

#include "driverlib.h"

void main (void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);

    //Enable A/D channel A0
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
        GPIO_PIN0
        );

    //Initialize the ADC12_A Module
    /*
     * Base address of ADC12_A Module
     * Use internal ADC12_A bit as sample/hold signal to start conversion
     * USE MODOSC 5MHZ Digital Oscillator as clock source
     * Use default clock divider of 1
     */
    ADC12_A_init(ADC12_A_BASE,
        ADC12_A_SAMPLEHOLDSOURCE_SC,
        ADC12_A_CLOCKSOURCE_ADC12OSC,
        ADC12_A_CLOCKDIVIDER_1);

   ADC12_A_enable(ADC12_A_BASE);

    /*
     * Base address of ADC12_A Module
     * For memory buffers 0-7 sample/hold for 16 clock cycles
     * For memory buffers 8-15 sample/hold for 4 clock cycles (default)
     * Disable Multiple Sampling
     */
    ADC12_A_setupSamplingTimer(ADC12_A_BASE,
        ADC12_A_CYCLEHOLD_16_CYCLES,
        ADC12_A_CYCLEHOLD_4_CYCLES,
        ADC12_A_MULTIPLESAMPLESDISABLE);

    //Configure Memory Buffer
    /*
     * Base address of the ADC12_A Module
     * Configure memory buffer 0
     * Map input A0 to memory buffer 0
     * Vref+ = VeREF+ (ext)
     * Vref- = AVss
     * Memory buffer 0 is not the end of a sequence
     */
    ADC12_A_configureMemoryParam param = {0};
	param.memoryBufferControlIndex = ADC12_A_MEMORY_0;
	param.inputSourceSelect = ADC12_A_INPUT_A0;
	param.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_EXT;
	param.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
	param.endOfSequence = ADC12_A_NOTENDOFSEQUENCE;
	ADC12_A_configureMemory(ADC12_A_BASE ,&param);
	
    while (1)
    {
        //Enable/Start first sampling and conversion cycle
        /*
         * Base address of ADC12_A Module
         * Start the conversion into memory buffer 0
         * Use the single-channel, single-conversion mode
         */
        ADC12_A_startConversion(ADC12_A_BASE,
            ADC12_A_MEMORY_0,
            ADC12_A_SINGLECHANNEL);

        //Poll for interrupt on memory buffer 0
        while (!ADC12_A_getInterruptStatus(ADC12_A_BASE,
                   ADC12IFG0)) ;

        //SET BREAKPOINT HERE
        __no_operation();
    }
}

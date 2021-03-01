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

//******************************************************************************
//!
//! \addtogroup ref_examples_list
//! REF - ADC12, Using REF Module to control Internal Reference
//!
//!  This example shows how to configure the internal reference of REF module
//!  using the REFCTL0 control register. The ADC12 uses the internal 2.5V
//!  reference and performs a single conversion on channel A0. The conversion
//!  results are stored in ADC12MEM0. Test by applying a voltage to channel A0,
//!  then setting and running to a break point at the "__no_operation()"
//!  instruction. To view the conversion results, open an ADC12 register window
//!  in debugger and view the contents of ADC12MEM0.
//!
//!  NOTE: REFMSTR bit in REFCTL0 regsiter is by default set to 1. This allows
//!  the REFCTL0 control regsiters to configure the reference setting. The
//!  legacy ADC12 reference control bits ADC12REFON, ADC12REF2_5, ADC12TCOFF
//!  and ADC12REFOUT are dont care in this case.
//!
//!      	  Tested on MSP430F5529
//!             -----------------
//!         /|\|                 |
//!          | |                 |
//!          --|RST              |
//!            |                 |
//!            |          P6.0/A0|<- Vin
//!            |                 |
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - REF peripheral
//! - ADC12 peripheral
//! - GPIO Port peripheral
//! - A0
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - None.
//!
//******************************************************************************

void main (void)
{
    volatile uint16_t i;

    //Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    //Enable A/D channel A0
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P6,
        GPIO_PIN0
        );


    //By default, REFMSTR=1 => REFCTL is used to configure the internal reference

    //If ref generator busy, WAIT
    while (Ref_isRefGenBusy(REF_BASE)) ;
    //Select internal ref = 2.5V
    Ref_setReferenceVoltage(REF_BASE,
        REF_VREF2_5V);
    //Internal Reference ON
    Ref_enableReferenceVoltage(REF_BASE);

    //Delay (~75us) for Ref to settle
    __delay_cycles(75);

    //Initialize the ADC12 Module
    /*
     * Base address of ADC12 Module
     * Use internal ADC12 bit as sample/hold signal to start conversion
     * USE MODOSC 5MHZ Digital Oscillator as clock source
     * Use default clock divider of 1
     */
    ADC12_A_init(ADC12_A_BASE,
        ADC12_A_SAMPLEHOLDSOURCE_SC,
        ADC12_A_CLOCKSOURCE_ADC12OSC,
        ADC12_A_CLOCKDIVIDER_1);

    ADC12_A_enable(ADC12_A_BASE);

    /*
     * Base address of ADC12 Module
     * For memory buffers 0-7 sample/hold for 64 clock cycles
     * For memory buffers 8-15 sample/hold for 4 clock cycles (default)
     * Disable Multiple Sampling
     */
    ADC12_A_setupSamplingTimer(ADC12_A_BASE,
        ADC12_A_CYCLEHOLD_64_CYCLES,
        ADC12_A_CYCLEHOLD_4_CYCLES,
        ADC12_A_MULTIPLESAMPLESENABLE);

    //Configure Memory Buffer
    /*
     * Base address of the ADC12 Module
     * Configure memory buffer 0
     * Map input A0 to memory buffer 0
     * Vref+ = Vref+ (INT)
     * Vref- = AVss
     */
    ADC12_A_configureMemoryParam configureMemoryParam = {0};
	configureMemoryParam.memoryBufferControlIndex = ADC12_A_MEMORY_0;
	configureMemoryParam.inputSourceSelect = ADC12_A_INPUT_A0;
	configureMemoryParam.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_INT;
	configureMemoryParam.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
	configureMemoryParam.endOfSequence = ADC12_A_NOTENDOFSEQUENCE;
	ADC12_A_configureMemory(ADC12_A_BASE ,&configureMemoryParam);
	

    while (1)
    {
        //Enable/Start sampling and conversion
        /*
         * Base address of ADC12 Module
         * Start the conversion into memory buffer 0
         * Use the single-channel, single-conversion mode
         */
        ADC12_A_startConversion(ADC12_A_BASE,
            ADC12_A_MEMORY_0,
            ADC12_A_SINGLECHANNEL);

        //Poll for interrupt on memory buffer 0
        while (ADC12IFG0 != ADC12_A_getInterruptStatus(ADC12_A_BASE,
                   ADC12IFG0)) ;

        //SET BREAKPOINT HERE
        __no_operation();
    }
}


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
//! ADC12_A - Sample A0-3 Input, AVcc Ref, Repeated Sequence of Conversions
//!
//!  This example shows how to perform a repeated sequence of conversions using
//!  "repeat sequence-of-channels" mode.  AVcc is used for the reference and
//!  repeated sequence of conversions is performed on Channels A0, A1, A2, and
//!  A3. Each conversion result is stored in ADC12MEM0, ADC12MEM1, ADC12MEM2,
//!  and ADC12MEM3 respectively. After each sequence, the 4 conversion results
//!  are moved to A0results[], A1results[], A2results[], and A3results[]. Test
//!  by applying voltages to channels A0 - A3. Open a watch window in debugger
//!  and view the results. Set Breakpoint1 in the index increment line to see
//!  the array values change sequentially and Breakpoint2 to see the entire
//!  array of conversion results in A0results[], A1results[], A2results[], and
//!  A3results[]for the specified Num_of_Results.
//!
//!  Note that a sequence has no restrictions on which channels are converted.
//!  For example, a valid sequence could be A0, A3, A2, A4, A2, A1, A0, and A7.
//!  See the MSP430x5xx User's Guide for instructions on using the ADC12_A.
//!
//!                MSP430F552x
//!             -----------------
//!         /|\|                 |
//!          | |          P6.0/A0|<- Vin0
//!          --|RST       P6.1/A1|<- Vin1
//!            |          P6.2/A2|<- Vin2
//!            |          P6.3/A3|<- Vin3
//!            |                 |Vref is Vcc in all cases
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - ADC12_A peripheral
//! - GPIO Port peripheral
//! - A0
//! - A1
//! - A2
//! - A3
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - ADC12_A_VECTOR
//!
//******************************************************************************
#include "driverlib.h"

#define   Num_of_Results   8

volatile uint16_t A0results[Num_of_Results];
volatile uint16_t A1results[Num_of_Results];
volatile uint16_t A2results[Num_of_Results];
volatile uint16_t A3results[Num_of_Results];

void main (void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);

    //Enable A/D channel inputs
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
        GPIO_PIN0 + GPIO_PIN1 + GPIO_PIN2 +
        GPIO_PIN3 + GPIO_PIN4 + GPIO_PIN5 +
        GPIO_PIN6 + GPIO_PIN7
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
        ADC12_A_CLOCKDIVIDER_1
        );

    ADC12_A_enable(ADC12_A_BASE);

    /*
     * Base address of ADC12_A Module
     * For memory buffers 0-7 sample/hold for 256 clock cycles
     * For memory buffers 8-15 sample/hold for 4 clock cycles (default)
     * Enable Multiple Sampling
     */
    ADC12_A_setupSamplingTimer(ADC12_A_BASE,
        ADC12_A_CYCLEHOLD_256_CYCLES,
        ADC12_A_CYCLEHOLD_4_CYCLES,
        ADC12_A_MULTIPLESAMPLESENABLE);

    //Configure Memory Buffers
    /*
     * Base address of the ADC12_A Module
     * Configure memory buffer 0
     * Map input A0 to memory buffer 0
     * Vref+ = AVcc
     * Vref- = AVss
     * Memory buffer 0 is not the end of a sequence
     */
    ADC12_A_configureMemoryParam param0 = {0};
	param0.memoryBufferControlIndex = ADC12_A_MEMORY_0;
	param0.inputSourceSelect = ADC12_A_INPUT_A0;
	param0.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_AVCC;
	param0.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
	param0.endOfSequence = ADC12_A_NOTENDOFSEQUENCE;
	ADC12_A_configureMemory(ADC12_A_BASE ,&param0);
		
    /*
     * Base address of the ADC12_A Module
     * Configure memory buffer 1
     * Map input A1 to memory buffer 1
     * Vref+ = AVcc
     * Vref- = AVss
     * Memory buffer 1 is not the end of a sequence
     *
     */
    ADC12_A_configureMemoryParam param1 = {0};
	param1.memoryBufferControlIndex = ADC12_A_MEMORY_1;
	param1.inputSourceSelect = ADC12_A_INPUT_A1;
	param1.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_AVCC;
	param1.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
	param1.endOfSequence = ADC12_A_NOTENDOFSEQUENCE;
	ADC12_A_configureMemory(ADC12_A_BASE ,&param1);
    /*
     * Base address of the ADC12_A Module
     * Configure memory buffer 2
     * Map input A2 to memory buffer 2
     * Vref+ = AVcc
     * Vref- = AVss
     * Memory buffer 2 is not the end of a sequence
     */
    ADC12_A_configureMemoryParam param2 = {0};
	param2.memoryBufferControlIndex = ADC12_A_MEMORY_2;
	param2.inputSourceSelect = ADC12_A_INPUT_A2;
	param2.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_AVCC;
	param2.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
	param2.endOfSequence = ADC12_A_NOTENDOFSEQUENCE;
	ADC12_A_configureMemory(ADC12_A_BASE ,&param2);
    /*
     * Base address of the ADC12_A Module
     * Configure memory buffer 3
     * Map input A3 to memory buffer 3
     * Vr+ = AVcc
     * Vr- = AVss
     * Memory buffer 3 IS the end of a sequence
     */
    ADC12_A_configureMemoryParam param3 = {0};
	param3.memoryBufferControlIndex = ADC12_A_MEMORY_3;
	param3.inputSourceSelect = ADC12_A_INPUT_A3;
	param3.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_AVCC;
	param3.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
	param3.endOfSequence = ADC12_A_ENDOFSEQUENCE;
	ADC12_A_configureMemory(ADC12_A_BASE ,&param3);

    //Enable memory buffer 3 interrupt
	ADC12_A_clearInterrupt(ADC12_A_BASE,
        ADC12IFG3);
    ADC12_A_enableInterrupt(ADC12_A_BASE,
        ADC12IE3);

    //Enable/Start first sampling and conversion cycle
    /*
     * Base address of ADC12_A Module
     * Start the conversion into memory buffer 0
     * Use the repeated sequence of channels
     */
    ADC12_A_startConversion(ADC12_A_BASE,
        ADC12_A_MEMORY_0,
        ADC12_A_REPEATED_SEQOFCHANNELS);

    //Enter LPM0, Enable interrupts
    __bis_SR_register(LPM0_bits + GIE);
    //For debugger
    __no_operation();
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC12_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(ADC12_VECTOR)))
#endif
void ADC12ISR (void)
{
    static uint16_t index = 0;

    switch (__even_in_range(ADC12IV,34)){
        case  0: break;   //Vector  0:  No interrupt
        case  2: break;   //Vector  2:  ADC overflow
        case  4: break;   //Vector  4:  ADC timing overflow
        case  6: break;   //Vector  6:  ADC12IFG0
        case  8: break;   //Vector  8:  ADC12IFG1
        case 10: break;   //Vector 10:  ADC12IFG2
        case 12:          //Vector 12:  ADC12IFG3
            //Move A0 results, IFG is cleared
            A0results[index] =
                ADC12_A_getResults(ADC12_A_BASE,
                    ADC12_A_MEMORY_0);
            //Move A1 results, IFG is cleared
            A1results[index] =
                ADC12_A_getResults(ADC12_A_BASE,
                    ADC12_A_MEMORY_1);
            //Move A2 results, IFG is cleared
            A2results[index] =
                ADC12_A_getResults(ADC12_A_BASE,
                    ADC12_A_MEMORY_2);
            //Move A3 results, IFG is cleared
            A3results[index] =
                ADC12_A_getResults(ADC12_A_BASE,
                    ADC12_A_MEMORY_3);

            //Increment results index, modulo; Set BREAKPOINT here
            index++;

            if (index == 8){
                (index = 0);
            }
        case 14: break;   //Vector 14:  ADC12IFG4
        case 16: break;   //Vector 16:  ADC12IFG5
        case 18: break;   //Vector 18:  ADC12IFG6
        case 20: break;   //Vector 20:  ADC12IFG7
        case 22: break;   //Vector 22:  ADC12IFG8
        case 24: break;   //Vector 24:  ADC12IFG9
        case 26: break;   //Vector 26:  ADC12IFG10
        case 28: break;   //Vector 28:  ADC12IFG11
        case 30: break;   //Vector 30:  ADC12IFG12
        case 32: break;   //Vector 32:  ADC12IFG13
        case 34: break;   //Vector 34:  ADC12IFG14
        default: break;
    }
}

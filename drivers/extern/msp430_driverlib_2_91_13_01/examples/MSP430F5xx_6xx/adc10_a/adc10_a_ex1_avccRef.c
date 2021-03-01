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
//! ADC10_A - Sample A0 Input, AVcc Ref, LED ON if A0 > 0.5AVcc
//!
//!  MSP430F550x Demo
//!  A single sample is made on A0 with reference to AVcc. Software sets ADC10SC
//!  to start sample and conversion - ADC10SC automatically cleared at EOC.ADC10_A
//!  internal oscillator times sample (16x) and conversion. In Mainloop MSP430
//!  waits in LPM0 to save power until ADC10_A conversion complete, ADC10_A_ISR will
//!  force exit from LPM0 in Mainloop on reti. If A0 > 0.5*AVcc, P1.0 set, else
//!  reset.
//!
//!                MSP430F550x
//!             -----------------
//!         /|\|              XIN|-
//!          | |                 |
//!          --|RST          XOUT|-
//!            |          P6.0/A0|<- Sample Signal
//!            |             P1.0|-> LED
//!            |                 |
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - ADC10_A peripheral
//! - GPIO Port peripheral
//! - A1
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - ADC10_A_VECTOR
//!
//******************************************************************************
#include "driverlib.h"

void main (void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    //Set P1.0 to output direction
    GPIO_setAsOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN0
        );

    //Initialize the ADC10_A Module
    /*
     * Base Address for the ADC10_A Module
     * Use internal ADC10_A bit as sample/hold signal to start conversion
     * USE MODOSC 5MHZ Digital Oscillator as clock source
     * Use default clock divider of 1
     */
    ADC10_A_init(ADC10_A_BASE,
        ADC10_A_SAMPLEHOLDSOURCE_SC,
        ADC10_A_CLOCKSOURCE_ADC10OSC,
        ADC10_A_CLOCKDIVIDER_1);

    ADC10_A_enable(ADC10_A_BASE);

    /*
     * Base Address for the ADC10_A Module
     * Sample/hold for 16 clock cycles
     * Do not enable Multiple Sampling
     */
    ADC10_A_setupSamplingTimer(ADC10_A_BASE,
        ADC10_A_CYCLEHOLD_16_CYCLES,
        ADC10_A_MULTIPLESAMPLESDISABLE);

    //Configure the Memory Buffer
    /*
     * Base Address for the ADC10_A Module
     * Use input A0
     * Use positive reference of AVcc
     * Use negative reference of AVss
     */
    ADC10_A_configureMemory(ADC10_A_BASE,
        ADC10_A_INPUT_A0,
        ADC10_A_VREFPOS_AVCC,
        ADC10_A_VREFNEG_AVSS);

    //Enable the Memory Buffer Interrupt
	ADC10_A_clearInterrupt(ADC10_A_BASE,
        ADC10IFG0);
    ADC10_A_enableInterrupt(ADC10_A_BASE,
        ADC10IE0);

    for (;;)
    {
        __delay_cycles(5000);

        //Enable and Start the conversion
        //in Single-Channel, Single Conversion Mode
        ADC10_A_startConversion(ADC10_A_BASE,
            ADC10_A_SINGLECHANNEL);

        //LPM0, ADC10_A_ISR will force exit
        __bis_SR_register(CPUOFF + GIE);
        __no_operation();


    }
}

// ADC10_A interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC10_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(ADC10_VECTOR)))
#endif
void ADC10_A_ISR (void) {

	switch (__even_in_range(ADC10IV,12)){
        case  0: break; //No interrupt
        case  2: break; //conversion result overflow
        case  4: break; //conversion time overflow
        case  6: break; //ADC10HI
        case  8: break; //ADC10LO
        case 10: break; //ADC10IN
        case 12:        //ADC10IFG0
            //(Automatically clears ADC10IFG0 by reading memory buffer)
           	if (ADC10_A_getResults(ADC10_A_BASE) < 0x1FF){
	            //Clear P1.0 LED off
	            GPIO_setOutputLowOnPin(
	                GPIO_PORT_P1,
	                GPIO_PIN0
	                );
	        } else   {
	            //Set P1.0 LED on
	            GPIO_setOutputHighOnPin(
	                GPIO_PORT_P1,
	                GPIO_PIN0
	                );
	        }

            //Clear CPUOFF bit from 0(SR)
            __bic_SR_register_on_exit(CPUOFF);
            break;
        default: break;
    }
}

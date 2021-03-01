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
//! ADC10_A - Sample A0 input, 1.5V Shared Ref, LED ON if A0 > 0.5V
//!
//!  MSP430F550x Demo
//!  A single sample is made on A0 with reference to internal 1.5V Vref.Software
//!  sets ADC10SC to start sample and conversion - ADC10SC automatically cleared
//!  at EOC. ADC10_A internal oscillator times sample (16x) and conversion. In
//!  Mainloop MSP430 waits in LPM0 to save power until ADC10_A conversion complete,
//!  ADC10_A_ISR will force exit from LPM0 in Mainloop on reti. If A0 > 0.5V, P1.0
//!  set, else reset.
//!
//!                MSP430F550x
//!             -----------------
//!         /|\|              XIN|-
//!          | |                 |
//!          --|RST          XOUT|-
//!            |          P6.0/A0|<- Sample Signal
//!            |             P1.0|-> LED
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - ADC10_A peripheral
//! - REF peripheral
//! - TIMER_A peripheral
//! - GPIO Port peripheral
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - ADC10_A_VECTOR
//! - TIMER0_A0_VECTOR
//!
//******************************************************************************
#include "driverlib.h"

#define TIMER_A_PERIOD 80

void main (void)
{
    //Stop Watchdog Timer
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

    //Configure Memory Buffer
    /*
     * Base Address for the ADC10_A Module
     * Use input A0
     * Use positive reference of Internally generated Vref
     * Use negative reference of AVss
     */
    ADC10_A_configureMemory(ADC10_A_BASE,
        ADC10_A_INPUT_A0,
        ADC10_A_VREFPOS_INT,
        ADC10_A_VREFNEG_AVSS);

    //Enable Memory Buffer interrupt
    ADC10_A_clearInterrupt(ADC10_A_BASE,
        ADC10IFG0);
    ADC10_A_enableInterrupt(ADC10_A_BASE,
        ADC10IE0);

    //Configure internal reference
    //If ref generator busy, WAIT
    while ( REF_BUSY == Ref_isRefGenBusy(REF_BASE) ) ;
    //Select internal ref = 1.5V
    Ref_setReferenceVoltage(REF_BASE,
        REF_VREF1_5V);
    //Internal Reference ON
    Ref_enableReferenceVoltage(REF_BASE);

    //Configure TA0 to provide delay for reference settling ~75us
    //Start timer in Up Mode with Capture compare interrupt enabled
   	Timer_A_initUpModeParam initUpParam = {0};
    initUpParam.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    initUpParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    initUpParam.timerPeriod = TIMER_A_PERIOD;
    initUpParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    initUpParam.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    initUpParam.timerClear = TIMER_A_SKIP_CLEAR;
    initUpParam.startTimer = true;
    Timer_A_initUpMode(TIMER_A1_BASE, &initUpParam);


    //LPM0, TA0_ISR will force exit
    __bis_SR_register(CPUOFF + GIE);

    //Disable timer Interrupt
    Timer_A_disableCaptureCompareInterrupt(TIMER_A0_BASE,
        TIMER_A_CAPTURECOMPARE_REGISTER_0);

    for (;;)
    {
        //Delay between conversions
        __delay_cycles(5000);

        //Enable and Start the conversion
        //in Single-Channel, Single Conversion Mode
        ADC10_A_startConversion(ADC10_A_BASE,
            ADC10_A_SINGLECHANNEL);

        //LPM0, ADC10_A_ISR will force exit
        __bis_SR_register(CPUOFF + GIE);
        //For debug only
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

            //Automatically clears ADC10IFG0 by reading memory buffer
			//ADC10MEM = A0 > 0.5V?
        	if (ADC10_A_getResults(ADC10_A_BASE) < 0x155){
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
            //Breakpoint here and watch ADC_Result
            __bic_SR_register_on_exit(CPUOFF);
            break;
        default: break;
    }
}

// TIMER_A interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER0_A0_VECTOR)))
#endif
void TA0_ISR (void) {

    Timer_A_stop(TIMER_A0_BASE);
    //Exit LPM0 on return
    LPM0_EXIT;
}

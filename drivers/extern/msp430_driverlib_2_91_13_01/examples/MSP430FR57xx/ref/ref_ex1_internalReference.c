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
//! REF - Sample A1 input, 1.5V Shared Ref, LED ON if A1 > 0.5V
//!
//!  MSP430FR573x Demo
//!  A single sample is made on A1 with reference to internal 1.5V Vref.Software
//!  sets ADC10SC to start sample and conversion - ADC10SC automatically cleared
//!  at EOC. ADC10B internal oscillator times sample (16x) and conversion. In
//!  Mainloop MSP430 waits in LPM0 to save power until ADC10B conversion complete,
//!  ADC10_ISR will force exit from LPM0 in Mainloop on reti. If A1 > 0.5V, P1.0
//!  set, else reset.
//!
//!                MSP430FR5739
//!             -----------------
//!         /|\|              XIN|-
//!          | |                 |
//!          --|RST          XOUT|-
//!            |          P1.1/A1|<- Sample Signal
//!            |             P1.0|-> LED
//!			   |             P3.5|-->LED
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - ADC10 peripheral
//! - REF peripheral
//! - Timer_A peripheral
//! - GPIO Port peripheral
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - ADC10_VECTOR
//! - TIMER0_A0_VECTOR
//!
//!   Built with IAR Embedded Workbench Version: 5.40 & Code Composer Studio V5.1
//******************************************************************************

#define TIMER_PERIOD 80

void main (void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);

    //Set P1.1 as an input pin.
    /*

     * Select Port 1
     * Set Pin 1 as input
     * Set Ternary module function
     */
	GPIO_setAsPeripheralModuleFunctionInputPin(
			GPIO_PORT_P1,
			GPIO_PIN1,
			GPIO_TERNARY_MODULE_FUNCTION);

	//Set P1.0 as an output pin.
    /*

     * Select Port 1
     * Set Pin 0 as output
     */
	GPIO_setAsOutputPin(
			GPIO_PORT_P1,
			GPIO_PIN0);


	//Set P3.5 as an output pin.
    /*

     * Select Port 3
     * Set Pin 5 as output
     */
	GPIO_setAsOutputPin(
			GPIO_PORT_P3,
			GPIO_PIN5);

    //Initialize the ADC10 Module
    /*
     * Base Address for the ADC10 Module
     * Use internal ADC10 bit as sample/hold signal to start conversion
     * USE MODOSC 5MHZ Digital Oscillator as clock source
     * Use default clock divider of 1
     */
    ADC10_B_init(ADC10_B_BASE,
        ADC10_B_SAMPLEHOLDSOURCE_SC,
        ADC10_B_CLOCKSOURCE_ADC10OSC,
        ADC10_B_CLOCKDIVIDER_1);

    ADC10_B_enable(ADC10_B_BASE);

    /*
     * Base Address for the ADC10 Module
     * Sample/hold for 16 clock cycles
     * Do not enable Multiple Sampling
     */
    ADC10_B_setupSamplingTimer(ADC10_B_BASE,
        ADC10_B_CYCLEHOLD_16_CYCLES,
        ADC10_B_MULTIPLESAMPLESDISABLE);

    //Configure Memory Buffer
    /*
     * Base Address for the ADC10 Module
     * Use input A1
     * Use positive reference of Internally generated Vref
     * Use negative reference of AVss
     */
    ADC10_B_configureMemory(ADC10_B_BASE,
        ADC10_B_INPUT_A1,
        ADC10_B_VREFPOS_INT,
        ADC10_B_VREFNEG_AVSS);

	ADC10_B_clearInterrupt(ADC10_B_BASE,
        ADC10IE0);

    //Enable Memory Buffer interrupt
    ADC10_B_enableInterrupt(ADC10_B_BASE,
        ADC10IE0);

    //Configure internal reference
    //If ref generator busy, WAIT
    while ( REF_BUSY == Ref_isRefGenBusy(REF_BASE) ) ;

    //Select internal ref = 1.5V
    Ref_setReferenceVoltage(REF_BASE,
        REF_VREF1_5V);
    //Internal Reference ON
    Ref_enableReferenceVoltage(REF_BASE);

    // Configure TA0 to provide delay for reference settling ~75us
    Timer_A_initUpModeParam param = {0};
    param.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    param.timerPeriod = TIMER_PERIOD;
    param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    param.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    param.timerClear = TIMER_A_DO_CLEAR;
    param.startTimer = true;
    Timer_A_initUpMode(TIMER_A0_BASE, &param);

    __bis_SR_register(CPUOFF + GIE);           // LPM0, TA0_ISR will force exit



    for (;;)
    {
        //Delay between conversions
        __delay_cycles(5000);

        //Enable and Start the conversion
        //in Single-Channel, Single Conversion Mode
        ADC10_B_startConversion(ADC10_B_BASE,
            ADC10_B_SINGLECHANNEL);

        //LPM0, ADC10_ISR will force exit
        __bis_SR_register(CPUOFF + GIE);
        //For debug only
        __no_operation();


    }
}

//ADC10 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC10_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(ADC10_VECTOR)))
#endif
void ADC10_ISR (void)
{
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
        	if (ADC10_B_getResults(ADC10_B_BASE) < 0x155){
           		//Clear P1.0 LED off
           		/*

           		* Select Port 1
           		* Set Pin 0 to output Low.
           		*/
           		GPIO_setOutputLowOnPin(
           			GPIO_PORT_P1,
           			GPIO_PIN0
           		);

				//Clear P3.5 LED off
           		/*

           		* Select Port 3
           		* Set Pin 5 to output Low.
           		*/
           		GPIO_setOutputLowOnPin(
           			GPIO_PORT_P3,
           			GPIO_PIN5
           		);

			} else   {
	            //Set P1.0 LED on
           		/*

           		* Select Port 1
           		* Set Pin 0 to output high.
           		*/
          		GPIO_setOutputHighOnPin(
           			GPIO_PORT_P1,
           			GPIO_PIN0
           		);

	            //Set P3.5 LED on
           		/*

           		* Select Port 3
           		* Set Pin 5 to output high.
           		*/
           		GPIO_setOutputHighOnPin(
           			GPIO_PORT_P3,
           			GPIO_PIN5
           		);
	        }

            //Clear CPUOFF bit from 0(SR)
            //Breakpoint here and watch ADC_Result
            __bic_SR_register_on_exit(CPUOFF);
            break;
        default: break;
    }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER0_A0_VECTOR)))
#endif
void TA0_ISR (void)
{
	  TA0CTL = 0;
	  LPM0_EXIT;                                // Exit LPM0 on return
}


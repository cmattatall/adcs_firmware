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
//! ADC10_A - Sample A1 Input, Window Comparator, 2.5V Internal Ref, TB0 Trigger
//!
//!  MSP430F550x Demo
//!  A1 is sampled in every 0.5s using the TA0.0 trigger in single ch/conversion
//!  mode. Window comparator is used to generate interrupts to indicate when the
//!  input voltage goes above the High_Threshold or below the Low_Threshold or
//!  is in between the high and low thresholds. TIMER_B0 is used as an interval
//!  timer used to control the LED at P1.0 to toggle slow/fast or turn off
//!  according to the ADC10_A Hi/Lo/IN interupts. ADC10_A is configured in pulse
//!  sample mode and is triggered by the TA0.0
//!
//!               MSP430F550x
//!            -----------------
//!        /|\|              XIN|-
//!         | |                 |
//!         --|RST          XOUT|-
//!           |          P6.1/A1|<- Sample Signal
//!           |             P1.0|-> LED [toggle slow/fast or turn off]
//!           |                 |
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - ADC10_A peripheral
//! - REF peripheral
//! - TIMER_B peripheral
//! - GPIO Port peripheral
//! - A1
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - ADC10_A_VECTOR
//! - TIMER0_B0_VECTOR
//!
//******************************************************************************
#include "driverlib.h"

//~2.0V Input
#define High_Threshold 818
//~1.0V Input
#define Low_Threshold 409

#define SlowToggle_Period  20000 - 1
#define FastToggle_Period  1000 - 1

Timer_B_initUpModeParam paramB2 = {0};

void main (void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);
	
	paramB2.clockSource = TIMER_B_CLOCKSOURCE_ACLK;
    paramB2.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1;
    paramB2.timerPeriod = FastToggle_Period;
    paramB2.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_DISABLE;
    paramB2.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_B_CCIE_CCR0_INTERRUPT_ENABLE;
    paramB2.timerClear = TIMER_B_SKIP_CLEAR;
    paramB2.startTimer = true;
    Timer_B_initUpMode(TIMER_B0_BASE, &paramB2);

    //Initialize the ADC10_A Module
    /*
     * Base Address for the ADC10_A Module
     * Use TIMER_A0 to signal a sample/hold-and-convert
     * USE MODOSC 5MHZ Digital Oscillator as clock source
     * Use default clock divider of 1
     */
    ADC10_A_init(ADC10_A_BASE,
        ADC10_A_SAMPLEHOLDSOURCE_1,
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
     * Use input A1
     * Use positive reference of Internally generated Vref
     * Use negative reference of AVss
     */
    ADC10_A_configureMemory(ADC10_A_BASE,
        ADC10_A_INPUT_A1,
        ADC10_A_VREFPOS_INT,
        ADC10_A_VREFNEG_AVSS);

    //Configure Window Comparator
    /*
     * Base Address for the ADC10_A Module
     * Set the high threshold of the window comparator
     * Set the low threshold of the window comparator
     */
    ADC10_A_setWindowComp(ADC10_A_BASE,
        High_Threshold,
        Low_Threshold);

    //Enable all window comparator interrupts
    ADC10_A_clearInterrupt(ADC10_A_BASE,
        ADC10HIIFG + ADC10LOIFG + ADC10INIFG);
    ADC10_A_enableInterrupt(ADC10_A_BASE,
        ADC10HIIE + ADC10LOIE + ADC10INIE);

    //Configure internal reference
    //If ref generator busy, WAIT
    while ( REF_BUSY == Ref_isRefGenBusy(REF_BASE) ) ;
    //Select internal ref = 1.5V
    Ref_setReferenceVoltage(REF_BASE,
        REF_VREF2_5V);
    //Internal Reference ON
    Ref_enableReferenceVoltage(REF_BASE);

    //Delay (~75us) for Ref to settle
    __delay_cycles(75);


    //Set P1.0 to output direction
    GPIO_setAsOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN0
        );

    //Clear P1.0 LED off
    GPIO_setOutputLowOnPin(
        GPIO_PORT_P1,
        GPIO_PIN0
        );

    //Configure TB0 period-timer
  	Timer_B_initUpModeParam paramB1 = {0};
    paramB1.clockSource = TIMER_B_CLOCKSOURCE_ACLK;
    paramB1.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1;
    paramB1.timerPeriod = 0;
    paramB1.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_DISABLE;
    paramB1.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_B_CCIE_CCR0_INTERRUPT_ENABLE;
    paramB1.timerClear = TIMER_B_DO_CLEAR;
    paramB1.startTimer = true;
    Timer_B_initUpMode(TIMER_B0_BASE, &paramB1);

    //Configure ADC10_A timer trigger TA0.1
   	Timer_A_initCompareModeParam initCompareModeParam = {0};
    initCompareModeParam.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    initCompareModeParam.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initCompareModeParam.compareOutputMode = TIMER_A_OUTPUTMODE_TOGGLE;
    initCompareModeParam.compareValue = 8192 - 1;
    Timer_A_initCompareMode(TIMER_A0_BASE, &initCompareModeParam);

	Timer_A_initUpModeParam paramA = {0};
    paramA.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
    paramA.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    paramA.timerPeriod = 16384 - 1;
    paramA.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    paramA.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    paramA.timerClear = TIMER_A_DO_CLEAR;
    paramA.startTimer = true;
    Timer_A_initUpMode(TIMER_A0_BASE, &paramA);


    //Enable and Start the conversion
    //in Repeated Single-Channel Conversion Mode
    ADC10_A_startConversion(ADC10_A_BASE,
        ADC10_A_REPEATED_SINGLECHANNEL);

    //Enter LPM3 w/ interrupts
    __bis_SR_register(LPM3_bits + GIE);
    //for debugger
    __no_operation();
}

// ADC10_A interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(ADC10_VECTOR)))
#endif
void ADC10_A_ISR (void) {

    switch (__even_in_range(ADC10IV,12)){
        case  0: break; //No interrupt
        case  2: break; //conversion result overflow
        case  4: break; //conversion time overflow
        case  6:        //ADC10HIIFG
            //Turn off Timer
            Timer_B_stop(TIMER_B0_BASE);

            //Set Timer Period for fast LED toggle
			
			paramB2.timerPeriod = FastToggle_Period;
			Timer_B_initUpMode(TIMER_B0_BASE, &paramB2);
            
            break;

        case  8:        //ADC10LOIFG
            //Turn off Timer
            Timer_B_stop(TIMER_B0_BASE);

            //Clear P1.0 LED off
            GPIO_setOutputLowOnPin(
            GPIO_PORT_P1,
            GPIO_PIN0
            );
            break;

        case 10:        //ADC10INIFG
            //Turn off Timer
            Timer_B_stop(TIMER_B0_BASE);

            //Set Timer Period for slow LED toggle
            paramB2.timerPeriod = SlowToggle_Period;
			Timer_B_initUpMode(TIMER_B0_BASE, &paramB2);
            
            break;
        case 12: break; //ADC10IFG0
        default: break;
    }
}

// TIMER_B interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER0_B0_VECTOR)))
#endif
void TIMER0_B0_ISR (void) {
    // Toggle P1.0
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

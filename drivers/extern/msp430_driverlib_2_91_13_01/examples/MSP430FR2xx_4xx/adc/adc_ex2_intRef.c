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
//! ADC - Sample A7 input, 1.5V Shared Ref, LED ON if A7 > 0.5V
//!
//!  MSP430FR4133 Demo
//!  A single sample is made on A7 with reference to internal 1.5V Vref.Software
//!  sets ADCSC to start sample and conversion - ADCSC automatically cleared
//!  at EOC. ADC internal oscillator times sample (16x) and conversion. In
//!  Mainloop MSP430 waits in LPM0 to save power until ADC conversion complete,
//!  ADC_ISR will force exit from LPM0 in Mainloop on reti. If A7 > 0.5V, P1.0
//!  set, else reset.
//!
//!           MSP430FR2xx_4xx Board
//!             -----------------
//!         /|\|              XIN|-
//!          | |                 |
//!          --|RST          XOUT|-
//!            |               A7|<-- Sample Signal
//!            |                 |-->LED1
//!            |                 |-->LED2
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - ADC peripheral
//! - REF peripheral
//! - Timer_A/Timer_B peripheral
//! - GPIO Port peripheral
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - ADC_VECTOR
//! - TIMER0_A0_VECTOR/TIMER0_B0_VECTOR
//!
//******************************************************************************
#include "driverlib.h"
#include "Board.h"
#define TIMER_PERIOD 80

void main (void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);

    //Set A7 as an input pin.
    //Set appropriate module function
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_ADC7,
            GPIO_PIN_ADC7,
            GPIO_FUNCTION_ADC7);

    //Set LED1 as an output pin.
    GPIO_setAsOutputPin(
            GPIO_PORT_LED1,
            GPIO_PIN_LED1);

    //Set LED2 as an output pin.
    GPIO_setAsOutputPin(
            GPIO_PORT_LED2,
            GPIO_PIN_LED2);

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    //Initialize the ADC Module
    /*
     * Base Address for the ADC Module
     * Use internal ADC bit as sample/hold signal to start conversion
     * USE MODOSC 5MHZ Digital Oscillator as clock source
     * Use default clock divider of 1
     */
    ADC_init(ADC_BASE,
        ADC_SAMPLEHOLDSOURCE_SC,
        ADC_CLOCKSOURCE_ADCOSC,
        ADC_CLOCKDIVIDER_1);

    ADC_enable(ADC_BASE);

    /*
     * Base Address for the ADC Module
     * Sample/hold for 16 clock cycles
     * Do not enable Multiple Sampling
     */
    ADC_setupSamplingTimer(ADC_BASE,
        ADC_CYCLEHOLD_16_CYCLES,
        ADC_MULTIPLESAMPLESDISABLE);

    //Configure Memory Buffer
    /*
     * Base Address for the ADC Module
     * Use input A7
     * Use positive reference of Internally generated Vref
     * Use negative reference of AVss
     */
    ADC_configureMemory(ADC_BASE,
        ADC_INPUT_A7,
        ADC_VREFPOS_INT,
        ADC_VREFNEG_AVSS);

    ADC_clearInterrupt(ADC_BASE,
        ADC_COMPLETED_INTERRUPT);

    //Enable Memory Buffer interrupt
    ADC_enableInterrupt(ADC_BASE,
        ADC_COMPLETED_INTERRUPT);

    //Internal Reference ON
    PMM_enableInternalReference();

    //Configure internal reference
    //If ref voltage no ready, WAIT
    while (PMM_REFGEN_NOTREADY == PMM_getVariableReferenceVoltageStatus());

#ifdef __MSP430_HAS_TBx__
    // Configure TB0 to provide delay for reference settling ~75us
    Timer_B_initUpModeParam initUpModeParam = {0};
    initUpModeParam.clockSource = TIMER_B_CLOCKSOURCE_SMCLK;
    initUpModeParam.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1;
    initUpModeParam.timerPeriod = TIMER_PERIOD;
    initUpModeParam.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_DISABLE;
    initUpModeParam.captureCompareInterruptEnable_CCR0_CCIE =
            TIMER_B_CCIE_CCR0_INTERRUPT_ENABLE;
    initUpModeParam.timerClear = TIMER_B_DO_CLEAR;
    initUpModeParam.startTimer = true;
    Timer_B_initUpMode(TB0_BASE, &initUpModeParam);
#else
    // Configure TA0 to provide delay for reference settling ~75us
    Timer_A_initUpModeParam initUpModeParam = {0};
    initUpModeParam.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    initUpModeParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    initUpModeParam.timerPeriod = TIMER_PERIOD;
    initUpModeParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    initUpModeParam.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    initUpModeParam.timerClear = TIMER_A_DO_CLEAR;
    initUpModeParam.startTimer = true;
    Timer_A_initUpMode(TIMER_A0_BASE, &initUpModeParam);
#endif  // __MSP430_HAS_TBx__

    __bis_SR_register(CPUOFF + GIE);           // LPM0, TA0_ISR will force exit

    for (;;)
    {
        //Delay between conversions
        __delay_cycles(5000);

        //Enable and Start the conversion
        //in Single-Channel, Single Conversion Mode
        ADC_startConversion(ADC_BASE,
                ADC_SINGLECHANNEL);

        //LPM0, ADC_ISR will force exit
        __bis_SR_register(CPUOFF + GIE);
        //For debug only
        __no_operation();


    }
}

//ADC interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(ADC_VECTOR)))
#endif
void ADC_ISR (void)
{
    switch (__even_in_range(ADCIV,12)){
        case  0: break; //No interrupt
        case  2: break; //conversion result overflow
        case  4: break; //conversion time overflow
        case  6: break; //ADCHI
        case  8: break; //ADCLO
        case 10: break; //ADCIN
        case 12:        //ADCIFG0

            //Automatically clears ADCIFG0 by reading memory buffer
            //ADCMEM = A0 > 0.5V?
            if (ADC_getResults(ADC_BASE) < 0x155) {
                //Turn LED1 off
                GPIO_setOutputLowOnPin(
                    GPIO_PORT_LED1,
                    GPIO_PIN_LED1
                );

                //Turn LED2 off
                GPIO_setOutputLowOnPin(
                    GPIO_PORT_LED2,
                    GPIO_PIN_LED2
                );
            }
            else {
                //Turn LED1 on
                GPIO_setOutputHighOnPin(
                    GPIO_PORT_LED1,
                    GPIO_PIN_LED1
                );

                //Turn LED2 on
                GPIO_setOutputHighOnPin(
                    GPIO_PORT_LED2,
                    GPIO_PIN_LED2
                );
            }

            //Clear CPUOFF bit from 0(SR)
            //Breakpoint here and watch ADC_Result
            __bic_SR_register_on_exit(CPUOFF);
            break;
        default: break;
    }
}

#ifdef __MSP430_HAS_TBx__
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_B0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER0_B0_VECTOR)))
#endif
void TB0_ISR (void)
{
      TB0CTL = 0;
      LPM0_EXIT;                                // Exit LPM0 on return
}
#else
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
#endif // __MSP430_HAS_TBx__

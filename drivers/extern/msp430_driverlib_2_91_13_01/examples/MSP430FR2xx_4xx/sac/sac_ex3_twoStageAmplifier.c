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
//!*****************************************************************************
//! SAC - SAC-L3, Unity-Gain Buffer Mode
//!
//! Description: Configure SAC-L3 for Unity-Gain Buffer Mode. The "+" terminal
//! is connected to OA0+. Op-amp's negative input is connected with its
//! positive input inside the silicon. SAC is set to low speed and low power mode.
//! ACLK = n/a, MCLK = SMCLK = default DCODIV ~1MHz.
//!
//!           MSP430FR2xx_4xx Board
//             -------------------
//         /|\|               OA0-|
//          | |               OA0O|
//          | |               OA0+|
//          --|RST                |
//            |               OA2-|
//            |               OA2O|
//            |               OA2+|<--
//            |               |   |
//            |             ADC A1|
//            |                   |
//            |               LED1|-->
//!
//! Wallace Tran
//! Texas Instruments Inc.
//! May 2018
//******************************************************************************
#include "Board.h"
#include "driverlib.h"

uint16_t adcResult;

int main(void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    //Setup LED1
    GPIO_setAsOutputPin(
        GPIO_PORT_LED1,
        GPIO_PIN_LED1);
    //Set LED1 low
    GPIO_setOutputLowOnPin(
        GPIO_PORT_LED1,
        GPIO_PIN_LED1);

    /*
     * Disable the GPIO power-on default high-impedance mode
     * to activate previously configured port settings
     */
    PMM_unlockLPM5();

    //Configure OA0 and OA2 functionality
    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_SACOA0O,
            GPIO_PIN_SACOA0O,
            GPIO_FUNCTION_SACOA0O);
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_SACOA0N,
            GPIO_PIN_SACOA0N,
            GPIO_FUNCTION_SACOA0N);
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_SACOA0P,
            GPIO_PIN_SACOA0P,
            GPIO_FUNCTION_SACOA0P);
    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_SACOA2O,
            GPIO_PIN_SACOA2O,
            GPIO_FUNCTION_SACOA2O);
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_SACOA2N,
            GPIO_PIN_SACOA2N,
            GPIO_FUNCTION_SACOA2N);
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_SACOA2P,
            GPIO_PIN_SACOA2P,
            GPIO_FUNCTION_SACOA2P);

    //Select external source for positive input
    //Select PGA source for negative input
    SAC_OA_init(
            SAC2_BASE,
            SAC_OA_POSITIVE_INPUT_SOURCE_EXTERNAL,
            SAC_OA_NEGATIVE_INPUT_SOURCE_PGA);
    //Select low speed and low power mode
    SAC_OA_selectPowerMode(
            SAC2_BASE,
            SAC_OA_POWER_MODE_LOW_SPEED_LOW_POWER);
    //Set gain=3
    SAC_PGA_setGain(
            SAC2_BASE,
            SAC_PGA_GAIN_BIT1);
    //Set as non-inverting PGA mode
    SAC_PGA_setMode(
            SAC2_BASE,
            SAC_PGA_MODE_NONINVERTING);
    //Enable SAC2 OA
    SAC_OA_enable(SAC2_BASE);
    //Enable SAC2
    SAC_enable(SAC2_BASE);

    //Select Pair OA source
    SAC_OA_init(
            SAC0_BASE,
            SAC_OA_POSITIVE_INPUT_SOURCE_PAIR_OA,
            SAC_OA_NEGATIVE_INPUT_SOURCE_PGA);
    //Select low speed and low power mode
    SAC_OA_selectPowerMode(
            SAC0_BASE,
            SAC_OA_POWER_MODE_LOW_SPEED_LOW_POWER);
    //Set gain=5
    SAC_PGA_setGain(
            SAC0_BASE,
            SAC_PGA_GAIN_BIT0 | SAC_PGA_GAIN_BIT1);
    //Set as non-inverting PGA mode
    SAC_PGA_setMode(
            SAC0_BASE,
            SAC_PGA_MODE_NONINVERTING);
    //Enable SAC0 OA
    SAC_OA_enable(SAC0_BASE);
    //Enable SAC0
    SAC_enable(SAC0_BASE);

    /*
     * Configure ADC12
     */
    ADC_init(
            ADC_BASE,
            ADC_SAMPLEHOLDSOURCE_SC,
            ADC_CLOCKSOURCE_ADCOSC,
            ADC_CLOCKDIVIDER_1);
    //S&H16 ADC clks
    ADC_setupSamplingTimer(
            ADC_BASE,
            ADC_CYCLEHOLD_16_CYCLES,
            ADC_MULTIPLESAMPLESDISABLE);
    //ADCON
    ADC_enable(ADC_BASE);
    //12-bit conversion results
    ADC_setResolution(
            ADC_BASE,
            ADC_RESOLUTION_12BIT);
    //Enable ADC converson complete interrupt
    ADC_enableInterrupt(
            ADC_BASE,
            ADC_COMPLETED_INTERRUPT);
    //A1 ADC input select = OA0 output
    //Vref = DVCC
    ADC_configureMemory(
            ADC_BASE,
            ADC_INPUT_A1,
            ADC_VREFPOS_AVCC,
            ADC_VREFNEG_AVSS);

    while(1)
    {
        //Sampling and conversion start
        ADC_startConversion(
                ADC_BASE,
                ADC_SINGLECHANNEL);

        // Enter LPM0, ADC_ISR will force exit
        __bis_SR_register(LPM0_bits | GIE);
        //For debug only
        __no_operation();

        if(adcResult > 2047)
        {
            GPIO_setOutputHighOnPin(
                    GPIO_PORT_LED1,
                    GPIO_PIN_LED1);
        }
        else
        {
            GPIO_setOutputLowOnPin(
                    GPIO_PORT_LED1,
                    GPIO_PIN_LED1);
        }
    }
}

// ADC interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC_VECTOR))) ADC_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(ADCIV, ADCIV_ADCIFG))
    {
        case ADCIV_NONE:
            break;
        case ADCIV_ADCOVIFG:
            break;
        case ADCIV_ADCTOVIFG:
            break;
        case ADCIV_ADCHIIFG:
            break;
        case ADCIV_ADCLOIFG:
            break;
        case ADCIV_ADCINIFG:
            break;
        case ADCIV_ADCIFG:
            //Read ADC memory
            adcResult = ADC_getResults(ADC_BASE);
            //Exit from LPM
            __bic_SR_register_on_exit(LPM0_bits);
            break;
        default:
            break;
    }
}


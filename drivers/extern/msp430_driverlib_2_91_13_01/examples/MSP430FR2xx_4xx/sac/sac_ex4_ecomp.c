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
//! SAC - SAC-L3, SAC0+SAC2+eCOMP0, use eCOMP0 to compare SAC0 PGA mode output
//!
//! Description: Configure SAC0 in Non-inverting PGA mode and its gain is 5.
//! Configure SAC2 in DAC buffer mode and DAC output is 1V.
//! Select 1.5V internal Vref as SAC2 DAC voltage reference.
//! Use eCOMP0 to compare SAC0 output and SAC2 output.
//! Add one adjustable DC voltage to OA0+ pin and observe LED status.
//! When OA0+ input voltage exceeds 200mV, CPOUT goes high and LED is turned on.
//! When OA0+ input voltage is less than 200mV then CPOUT goes low and LED is
//! turned off. eCOMP interrupt enabled, wake up from LPM3.
//! Both SAC0 and SAC2 are configured to low speed and low power mode.
//! ACLK = n/a, MCLK = SMCLK = default DCODIV ~1MHz.
//!
//!           MSP430FR2xx_4xx Board
//!            -------------------
//!        /|\|               OA0-|
//!         | |               OA0O|
//!         | |               OA0+|<--
//!         --|RST                |
//!           |               OA2-|
//!           |               OA2O|
//!           |               OA2+|
//!           |                   |
//!           |            COMP0.O|-->
//!           |               LED1|-->
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

    //Configure reference module
    //Enable internal 1.5V reference
    PMM_enableInternalReference();
    PMM_selectVoltageReference(PMM_REFVSEL_1_5V);
    //Poll until internal reference settles
    while(!PMM_getVariableReferenceVoltageStatus());

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

    //Select positive and negative pin input
    SAC_OA_init(
            SAC0_BASE,
            SAC_OA_POSITIVE_INPUT_SOURCE_EXTERNAL,
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

    //Select 1.5V internal Vref as SAC2 DAC reference
    SAC_DAC_selectRefVoltage(
            SAC2_BASE,
            SAC_DAC_SECONDARY_REFERENCE);
    //Set SAC2 DAC output = 1V
    SAC_DAC_setData(
            SAC2_BASE,
            0xAAA);
    //Enable SAC2 DAC
    SAC_DAC_enable(SAC2_BASE);
    //Select positive and negative pin input
    SAC_OA_init(
            SAC2_BASE,
            SAC_OA_POSITIVE_INPUT_SOURCE_DAC,
            SAC_OA_NEGATIVE_INPUT_SOURCE_PGA);
    //Select low speed and low power mode
    SAC_OA_selectPowerMode(
            SAC2_BASE,
            SAC_OA_POWER_MODE_LOW_SPEED_LOW_POWER);
    // Set OA as buffer mode
    SAC_PGA_setMode(
            SAC2_BASE,
            SAC_PGA_MODE_BUFFER);
    //Enable SAC2 OA
    SAC_OA_enable(SAC2_BASE);
    //Enable SAC2
    SAC_enable(SAC2_BASE);

    /*
     * Setup eCOMP
     */
    EComp_initParam param = {0};
    //Select SAC0 and SAC2 output as eCOMP0 input
    //Enable eCOMP input
    param.positiveTerminalInput = ECOMP_INPUT_DEVICE_SPECIFIC_1;
    param.negativeTerminalInput = ECOMP_INPUT_DEVICE_SPECIFIC_1;
    param.outputFilterEnableAndDelayLevel = ECOMP_FILTER_DELAY_OFF;
    param.invertedOutputPolarity = ECOMP_NORMAL_OUTPUT_POLARITY;
    EComp_init(ECOMP0_BASE, &param);
    //Enable eCOMP dual edge interrupt
    EComp_enableInterrupt(
            ECOMP0_BASE,
            ECOMP_OUTPUT_INTERRUPT |
            ECOMP_INVERTED_POLARITY_INTERRUPT);
    //Set low power mode
    EComp_selectPowerMode(
            ECOMP0_BASE,
            ECOMP_POWER_MODE_LOW_POWER_LOW_SPEED);
    //Turn on eCOMP
    EComp_enable(ECOMP0_BASE);

    //Enter LPM3
    __bis_SR_register(LPM3_bits | GIE);
    //For debug
    __no_operation();
}

// eCOMP interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ECOMP0_ECOMP1_VECTOR
__interrupt void ECOMP0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ECOMP0_ECOMP1_VECTOR))) ECOMP0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(CP0IV, CPIV__CPIIFG))
    {
        case CPIV__NONE:
            break;
        case CPIV__CPIFG:
            //OA0+ input > 200mV
            GPIO_setOutputHighOnPin(
                    GPIO_PORT_LED1,
                    GPIO_PIN_LED1);
            break;
        case CPIV__CPIIFG:
            //OA0+ input < 200mV
            GPIO_setOutputLowOnPin(
                    GPIO_PORT_LED1,
                    GPIO_PIN_LED1);
            break;
        default:
            break;
    }
}


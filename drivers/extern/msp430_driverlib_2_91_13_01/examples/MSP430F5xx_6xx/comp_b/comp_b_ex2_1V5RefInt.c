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
//! COMPB - CB Interrupt, Input compared against 1.5V Internal Reference
//!
//!  Use CompB and internal reference to determine if input 'Vcompare' is high
//!  of low.  For the first time, when Vcompare exceeds the 1.5V internal
//!  reference, CBIFG is set and device enters the CompB ISR. In the ISR CBIES
//!  is toggled such that when Vcompare is less than 1.5V internal reference,
//!  CBIFG is set. LED is toggled inside the CompB ISR
//!
//!                 MSP430x552x
//!             ------------------
//!         /|\|                  |
//!          | |                  |
//!          --|RST       P6.0/CB0|<- Vcompare
//!            |              P1.0|-> LED 'ON'(Vcompare>1.5V); 'OFF'(Vcompare<1.5V)
//!            |                  |
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - COMPB peripheral
//! - GPIO Port peripheral
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - COMP_B_B_VECTOR
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

    //Initialize the Comparator B module
    /*
     * Base Address of Comparator B,
     * Pin CB0 to Positive(+) Terminal
     * Reference Voltage to Negative(-) Terminal
     * Normal Power Mode
     * Output Filter On with minimal delay
     * Non-Inverted Output Polarity
     */
    Comp_B_initParam initParam = {0};
    initParam.positiveTerminalInput = COMP_B_INPUT0;
    initParam.negativeTerminalInput = COMP_B_VREF;
    initParam.powerModeSelect = COMP_B_POWERMODE_NORMALMODE;
    initParam.outputFilterEnableAndDelayLevel = COMP_B_FILTEROUTPUT_DLYLVL1;
    initParam.invertedOutputPolarity = COMP_B_NORMALOUTPUTPOLARITY;
	Comp_B_init(COMP_B_BASE, &initParam);

    //Set the reference voltage that is being supplied to the (-) terminal
    /*
     * Base Address of Comparator B,
     * Reference Voltage of 1.5 V,
     * Lower Limit of 1.5*(32/32) = 1.5V,
     * Upper Limit of 1.5*(32/32) = 1.5V,
     * Static Mode Accuracy
     */
	Comp_B_configureReferenceVoltageParam refVoltageParam = {0};
    refVoltageParam.supplyVoltageReferenceBase = COMP_B_VREFBASE1_5V;
    refVoltageParam.lowerLimitSupplyVoltageFractionOf32 = 32;
    refVoltageParam.upperLimitSupplyVoltageFractionOf32 = 32;
    refVoltageParam.referenceAccuracy = COMP_B_ACCURACY_STATIC;
    Comp_B_configureReferenceVoltage(COMP_B_BASE, &refVoltageParam);
    
    //Enable Interrupts
    /*
     * Base Address of Comparator B,
     * Enable CompB Interrupt on default rising edge for CBIFG
     */
	Comp_B_clearInterrupt(COMP_B_BASE,
        CBIFG);
    Comp_B_enableInterrupt(COMP_B_BASE,
        CBIE);

    //Allow power to Comparator module
    Comp_B_enable(COMP_B_BASE);

    //Enter LPM4 with inetrrupts enabled
    __bis_SR_register(LPM4_bits + GIE);

    //For debug
    __no_operation();
}

//******************************************************************************
//
//This is the COMP_B_VECTOR interrupt vector service routine.
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=COMP_B_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(COMP_B_VECTOR)))
#endif
void COMP_B_ISR (void)
{
    //Toggle the edge at which an interrupt is generated
    Comp_B_toggleInterruptEdgeDirection(COMP_B_BASE);

    //Clear Interrupt flag
    Comp_B_clearInterrupt(COMP_B_BASE, CBIFG);

    //Toggle P1.0
    GPIO_toggleOutputOnPin(
        GPIO_PORT_P1,
        GPIO_PIN0
        );
}

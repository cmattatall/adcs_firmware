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
//! COMPB - Hysteresis, Output Toggle in LPM4, High Speed Mode
//!
//!  Use CompB and shared reference to determine if input 'Vcompare' is high of
//!  low.  Shared reference is configured to generate hysteresis. When Vcompare
//!  exceeds Vcc*3/4 CBOUT goes high and when Vcompare is less than Vcc*1/4 then
//!  CBOUT goes low. Connect P1.6/CBOUT to P1.0 externally to see the LED toggle
//!  accordingly.
//!
//!           MSP430x552x
//!       -----------------
//!   /|\|                 |
//!    | |                 |
//!    --|RST      P6.0/CB0|<- Vcompare
//!      |       P1.6/CBOUT|--> 'high'(Vcompare>Vcc*3/4);'low'(Vcompare<Vcc*1/4)
//!      |                 | |
//!      |             P1.0|_|LED 'ON'(Vcompare>Vcc*3/4);'OFF'(Vcompare<Vcc*1/4)
//!      |                 |
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - COMPB peripheral
//! - GPIO Port peripheral
//! - CB0
//! - CBOUT
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - None.
//!
//******************************************************************************
#include "driverlib.h"

void main (void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    //Select CBOUT function on P1.6/CBOUT and set P1.6 output direction
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN6);
    //Initialize the Comparator B module
    /*
     * Base Address of Comparator B,
     * Pin CB0 to Positive(+) Terminal,
     * Reference Voltage to Negative(-) Terminal,
     * Ultra-Low Power Mode,
     * Output Filter On with minimal delay,
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
     * Reference Voltage of Vcc,
     * Lower Limit of Vcc*(8/32) = (1/4)*Vcc,
     * Upper Limit of Vcc*(24/32) = (3/4)*Vcc,
     * Static Mode Accuracy
     */
    Comp_B_configureReferenceVoltageParam refVoltageParam = {0};
    refVoltageParam.supplyVoltageReferenceBase = COMP_B_VREFBASE_VCC;
    refVoltageParam.lowerLimitSupplyVoltageFractionOf32 = 8;
    refVoltageParam.upperLimitSupplyVoltageFractionOf32 = 24;
    refVoltageParam.referenceAccuracy = COMP_B_ACCURACY_STATIC;
    Comp_B_configureReferenceVoltage(COMP_B_BASE, &refVoltageParam);

    //Allow power to Comparator module
    Comp_B_enable(COMP_B_BASE);

    //delay for the reference to settle
    __delay_cycles(75);

    //Enter LPM4
    __bis_SR_register(LPM4_bits);

    //For debug
    __no_operation();
}

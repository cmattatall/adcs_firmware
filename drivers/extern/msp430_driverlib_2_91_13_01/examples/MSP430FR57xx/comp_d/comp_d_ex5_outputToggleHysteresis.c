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
//  MSP430FR57xx Demo - COMPD Hysteresis, CDOUT Toggle in LPM4; High speed mode
//
//  Description: Use Compd and shared reference to determine if input 'Vcompare'
//  is high of low.  Shared reference is configured to generate hysteresis.
//  When Vcompare exceeds Vcc*3/4 CDOUT goes high and when Vcompare is less
//  than Vcc*1/4 then CDOUT goes low.
//  P3.5/CDOUT is connected internally to an LED; it will toggle accordingly.
//
//                 MSP430FR5739
//             ------------------
//         /|\|                  |
//          | |                  |
//          --|RST      P1.2/CD2 |<--Vcompare
//            |                  |
//            |        P3.5/CDOUT|----> 'high'(Vcompare>Vcc*3/4); 'low'(Vcompare<Vcc*1/4)
//            |                  |  |
//            |                  |  | LED 'ON'(Vcompare>Vcc*3/4); 'OFF'(Vcompare<Vcc*1/4)
//            |                  |
//
//******************************************************************************
#include "driverlib.h"

void main(void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

  	//Set P3.5 as Ternary Module Function Output.
  	/*
  	* Select Port 3
  	* Set Pin 5 to output Ternary Module Function, (CBOUT).
  	*/
  	GPIO_setAsPeripheralModuleFunctionOutputPin(
  		GPIO_PORT_P3,
  		GPIO_PIN5,
  		GPIO_TERNARY_MODULE_FUNCTION
  	);

  //Initialize the Comparator D module
    /* Base Address of Comparator D,
     * Pin CD2 to Positive(+) Terminal,
     * Reference Voltage to Negative(-) Terminal,
     * Normal Power Mode,
     * Output Filter On with minimal delay,
     * Non-Inverted Output Polarity
     */
    Comp_D_initParam param = {0};
	param.positiveTerminalInput = COMP_D_INPUT2;
	param.negativeTerminalInput = COMP_D_VREF;
	param.outputFilterEnableAndDelayLevel = COMP_D_FILTEROUTPUT_OFF;
	param.invertedOutputPolarity = COMP_D_NORMALOUTPUTPOLARITY;
    Comp_D_init(COMP_D_BASE, &param);

    //Set the reference voltage that is being supplied to the (-) terminal
    /* Base Address of Comparator D,
     * Reference Voltage of Vcc V,
	 * Lower Limit of Vcc*(8/32) = Vcc*1/4,
     * Upper Limit of Vcc*(24/32) = Vcc*3/4,
     * Static Mode Accuracy
     */
    Comp_D_setReferenceVoltage(COMP_D_BASE,
    	COMP_D_REFERENCE_AMPLIFIER_DISABLED,
        8,
        24
        );

    //Disable Input Buffer on P1.2/CD2
	/* Base Address of Comparator D,
	 * Input Buffer port
	 * Selecting the CDx input pin to the comparator
	 * multiplexer with the CDx bits automatically
	 * disables output driver and input buffer for
	 * that pin, regardless of the state of the
	 * associated CDPD.x bit
	 */
	  Comp_D_disableInputBuffer(COMP_D_BASE,
	  	  COMP_D_INPUT2);

    //Allow power to Comparator module
    Comp_D_enable(COMP_D_BASE);

  __delay_cycles(400);           // delay for the reference to settle

  __bis_SR_register(LPM4_bits); // Enter LPM4
  __no_operation();             // For debug
}

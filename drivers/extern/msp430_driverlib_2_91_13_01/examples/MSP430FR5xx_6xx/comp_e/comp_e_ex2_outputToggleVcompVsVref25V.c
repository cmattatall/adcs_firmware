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
// MSP430FR59xx Demo - COMPE Toggle from LPM4; input channel C2;
// 		Vcompare is compared against the internal reference 2.5V
//
// Description: Use CompE (input channel C2) and internal reference to
//	  determine if input'Vcompare'is high or low.  When Vcompare exceeds 2.5V
//    CEOUT goes high and when Vcompare is less than 2.5V then CEOUT goes low.
//    P3.5/COUT is connected internally to an LED; it will toggle accordingly.
//
//                 MSP430FR5969
//             ------------------
//         /|\|                  |
//          | |                  |
//          --|RST        P1.2/C2|<--Vcompare
//            |                  |
//            |         P3.5/COUT|----> 'high'(Vcompare>2.5V); 'low'(Vcompare<2.5V)
//            |                  |  |
//            |                  |  | LED 'ON'(Vcompare>2.5V); 'OFF'(Vcompare<2.5V)
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
	* Set Pin 5 to output Ternary Module Function, (COUT).
	*/
	GPIO_setAsPeripheralModuleFunctionOutputPin(
		GPIO_PORT_P3,
		GPIO_PIN5,
		GPIO_TERNARY_MODULE_FUNCTION
	);

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    //Initialize the Comparator E module
    /* Base Address of Comparator E,
     * Pin C2 to Positive(+) Terminal,
     * Reference Voltage to Negative(-) Terminal,
     * Normal Power Mode,
     * Output Filter On with minimal delay,
     * Non-Inverted Output Polarity
     */
    Comp_E_initParam param = {0};
    param.posTerminalInput = COMP_E_INPUT2;
    param.negTerminalInput = COMP_E_VREF;
    param.outputFilterEnableAndDelayLevel = COMP_E_FILTEROUTPUT_OFF;
    param.invertedOutputPolarity = COMP_E_NORMALOUTPUTPOLARITY;
    Comp_E_init(COMP_E_BASE, &param);

    //Set the reference voltage that is being supplied to the (-) terminal
    /* Base Address of Comparator E,
     * Reference Voltage of 2.5 V,
	 * Lower Limit of 2.5*(32/32) = 2.5V,
     * Upper Limit of 2.5*(32/32) = 2.5V
     */
    Comp_E_setReferenceVoltage(COMP_E_BASE,
        COMP_E_VREFBASE2_5V,
        32,
        32
        );

    //Disable Input Buffer on P1.2/CE2
	/* Base Address of Comparator E,
	 * Input Buffer port
	 * Selecting the CEx input pin to the comparator
	 * multiplexer with the CEx bits automatically
	 * disables output driver and input buffer for
	 * that pin, regardless of the state of the
	 * associated CEPD.x bit
	 */
    Comp_E_disableInputBuffer(COMP_E_BASE,
	  	  COMP_E_INPUT2);

    //Allow power to Comparator module
    Comp_E_enable(COMP_E_BASE);

    __delay_cycles(400);           // delay for the reference to settle

    __bis_SR_register(LPM4_bits);          // Enter LPM4
    __no_operation();                      // For debug
}


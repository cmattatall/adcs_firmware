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
// MSP430FR5969 Demo - COMPE interrupt capability; Vcompare is compared against
//                    internal 1.2V reference
//
// Description: Use COMPE and internal reference to determine if input'Vcompare'
// is high of low.  For the first time, when Vcompare exceeds the 1.2V internal
// reference, CEIFG is set and device enters the COMPE ISR. In the ISR, CEIES is
// toggled such that when Vcompare is less than 1.2V internal reference;
// CEIFG is set.
// LED is toggled inside the COMPE ISR
//
//                 MSP430FR5969
//             ------------------
//         /|\|                  |
//          | |                  |
//          --|RST      P3.1/CE13|<--Vcompare
//            |                  |
//            |              P3.4|----> 'high'(Vcompare>1.2V); 'low'(Vcompare<1.2V)
//            |                  |  |
//            |                  |  | LED 'ON'(Vcompare>1.2V); 'OFF'(Vcompare<1.2V)
//            |                  |
//
//******************************************************************************
#include "driverlib.h"

void main (void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    //Set P3.4 as an output pin.
    /*

     * Select Port 3
     * Set Pin 4 as output
     */
  	GPIO_setAsOutputPin(
  			GPIO_PORT_P3,
  			GPIO_PIN4
  	);
    //Set P3.4 as Output Low.
    /*

     * Select Port 3
     * Set Pin 4 to output Low.
     */
    GPIO_setOutputLowOnPin(
  			GPIO_PORT_P3,
  			GPIO_PIN4
  	);

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    //Initialize the Comparator E module
    /*
     * Base Address of Comparator E,
     * Pin CE13 to Positive(+) Terminal
     * Reference Voltage to Negative(-) Terminal
     * Normal Power Mode
     * Output Filter On with minimal delay
     * Non-Inverted Output Polarity
     */
    Comp_E_initParam param = {0};
    param.posTerminalInput = COMP_E_INPUT13;
    param.negTerminalInput = COMP_E_VREF;
    param.outputFilterEnableAndDelayLevel = COMP_E_FILTEROUTPUT_OFF;
    param.invertedOutputPolarity = COMP_E_NORMALOUTPUTPOLARITY;
    Comp_E_init(COMP_E_BASE, &param);

    //Set the reference voltage that is being supplied to the (-) terminal
    /*
     * Base Address of Comparator E,
     * Reference Voltage of 1.5 V,
	 * Lower Limit of 1.2*(32/32) = 1.2V,
     * Upper Limit of 1.2*(32/32) = 1.2V
     */
    Comp_E_setReferenceVoltage(COMP_E_BASE,
        COMP_E_VREFBASE1_2V,
        32,
        32
        );

    //Enable COMP_E Interrupt on default rising edge for CEIFG
    Comp_E_setInterruptEdgeDirection(COMP_E_BASE, COMP_E_RISINGEDGE);

    // Clear any erroneous interrupts
    Comp_E_clearInterrupt(COMP_E_BASE, (COMP_E_OUTPUT_INTERRUPT_FLAG + COMP_E_INTERRUPT_FLAG_INVERTED_POLARITY));

    //Enable Interrupts
    /*
     * Base Address of Comparator E,
     * Enable COMPE Interrupt on default rising edge for CEIFG
     */

	 Comp_E_clearInterrupt(COMP_E_BASE,
        COMP_E_OUTPUT_INTERRUPT_FLAG
        );

    Comp_E_enableInterrupt(COMP_E_BASE,
        COMP_E_OUTPUT_INTERRUPT
        );

    //Allow power to Comparator module
    Comp_E_enable(COMP_E_BASE);

    //Enter LPM4 with interrupts enabled
    __bis_SR_register(LPM4_bits + GIE);

    //For debug
    __no_operation();
}

//******************************************************************************
//
//This is the COMP_E_VECTOR interrupt vector service routine.
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=COMP_E_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(COMP_E_VECTOR)))
#endif
void COMP_E_ISR (void)
{
    //Toggle the edge at which an interrupt is generated
    Comp_E_toggleInterruptEdgeDirection(COMP_E_BASE);

    //Clear Interrupt flag
    Comp_E_clearInterrupt(COMP_E_BASE, COMP_E_OUTPUT_INTERRUPT_FLAG);

	//Toggle P3.4 output pin.
	/*

	* Select Port 3
	* Set Pin 4 as output
	*/
	GPIO_toggleOutputOnPin(
		GPIO_PORT_P3,
		GPIO_PIN4
	);
}

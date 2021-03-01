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
// MSP430FR57xx Demo - COMPD interrupt capability; Vcompare is compared against
//                    internal 1.5V reference
//
// Description: Use CompD and internal reference to determine if input'Vcompare'
// is high of low.  For the first time, when Vcompare exceeds the 1.5V internal
// reference, CDIFG is set and device enters the CompD ISR. In the ISR, CDIES is
// toggled such that when Vcompare is less than 1.5V internal reference;
// CDIFG is set.
// LED is toggled inside the CompD ISR
//
//                 MSP430FR5739
//             ------------------
//         /|\|                  |
//          | |                  |
//          --|RST      P3.1/CD13|<--Vcompare
//            |                  |
//            |              P3.4|----> 'high'(Vcompare>2.0V); 'low'(Vcompare<2.5V)
//            |                  |  |
//            |                  |  | LED 'ON'(Vcompare>2.0V); 'OFF'(Vcompare<2.5V)
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

    //Initialize the Comparator B module
    /*
     * Base Address of Comparator B,
     * Pin CD13 to Positive(+) Terminal
     * Reference Voltage to Negative(-) Terminal
     * Normal Power Mode
     * Output Filter On with minimal delay
     * Non-Inverted Output Polarity
     */
   	Comp_D_initParam param = {0};
	param.positiveTerminalInput = COMP_D_INPUT13;
	param.negativeTerminalInput = COMP_D_VREF;
	param.outputFilterEnableAndDelayLevel = COMP_D_FILTEROUTPUT_OFF;
	param.invertedOutputPolarity = COMP_D_NORMALOUTPUTPOLARITY;
    Comp_D_init(COMP_D_BASE, &param);

    //Set the reference voltage that is being supplied to the (-) terminal
    /*
     * Base Address of Comparator D,
     * Reference Voltage of 1.5 V,
	 * Lower Limit of 1.5*(32/32) = 1.5V,
     * Upper Limit of 1.5*(32/32) = 1.5V,
     * Static Mode Accuracy
     */
    Comp_D_setReferenceVoltage(COMP_D_BASE,
        COMP_D_VREFBASE1_5V,
        32,
        32
        );

    //Enable CompB Interrupt on default rising edge for CDIFG
    Comp_D_setInterruptEdgeDirection(COMP_D_BASE, COMP_D_RISINGEDGE);

    // Clear any erroneous interrupts
    Comp_D_clearInterrupt(COMP_D_BASE, (COMP_D_INTERRUPT_FLAG + COMP_D_INTERRUPT_FLAG_INVERTED_POLARITY));

    //Enable Interrupts
    /*
     * Base Address of Comparator B,
     * Enable CompB Interrupt on default rising edge for CDIFG
     */
	 Comp_D_clearInterrupt(COMP_D_BASE,
        COMP_D_INTERRUPT
        );

    Comp_D_enableInterrupt(COMP_D_BASE,
        COMP_D_INTERRUPT
        );

    //Allow power to Comparator module
    Comp_D_enable(COMP_D_BASE);

    //Enter LPM4 with interrupts enabled
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
#pragma vector=COMP_D_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(COMP_D_VECTOR)))
#endif
void COMP_D_D_ISR (void)
{
    //Toggle the edge at which an interrupt is generated
    Comp_D_toggleInterruptEdgeDirection(COMP_D_BASE);

    //Clear Interrupt flag
    Comp_D_clearInterrupt(COMP_D_BASE, COMP_D_INTERRUPT_FLAG);

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

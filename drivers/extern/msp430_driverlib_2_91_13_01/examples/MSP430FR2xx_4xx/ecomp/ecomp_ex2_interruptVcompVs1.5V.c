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
// MSP430FR2311 Demo - ECOMP interrupt capability; Vcompare is compared against
//                    internal 1.5V reference
//
// Description: Use ECOMP and internal reference to determine if input'Vcompare'
// is high of low.  For the first time, when Vcompare exceeds the 1.5V internal
// reference, CPIFG is set and device enters the ECOMP ISR. In the ISR, CPIES is
// toggled such that when Vcompare is less than 1.5V internal reference;
// CPIFG is set.
// LED(P1.0) which is externally connected to COUT is toggled inside the ECOMP ISR
//
//                 MSP430FR2311
//             ------------------
//         /|\|                  |
//          | |                  |
//          --|RST        P1.1/C1|<--Vcompare
//            |                  |
//            |         P2.0/COUT|----> 'high'(Vcompare>1.5V); 'low'(Vcompare<1.5V)
//            |                  |  |
//            |                  |  | LED 'ON'(Vcompare>1.5V); 'OFF'(Vcompare<1.5V)
//            |                  |
//
//******************************************************************************
#include "driverlib.h"

void main (void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    //Set P2.0 as Secondary Module Function Output.(COUT)
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_P2,
        GPIO_PIN0,
        GPIO_SECONDARY_MODULE_FUNCTION
    );

    //Set P1.0 as output pin
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    //Initialize the EComp module
    /* Base Address of EComp,
    * Pin C1 to Positive(+) Terminal,
    * Reference Voltage (from DAC) to Negative(-) Terminal,
    * Output Filter delay off,
    * Non-Inverted Output Polarity
    */
    EComp_initParam param = {0};
    param.positiveTerminalInput = ECOMP_INPUT_1;
    param.negativeTerminalInput = ECOMP_INPUT_DAC;
    param.outputFilterEnableAndDelayLevel = ECOMP_FILTER_DELAY_OFF;
    param.invertedOutputPolarity = ECOMP_NORMAL_OUTPUT_POLARITY;
    EComp_init(ECOMP0_BASE, &param);

    //Set the reference voltage that is outputed by built-in DAC
    EComp_configureDACParam dacParam = {0};
    dacParam.referenceVoltage = ECOMP_DAC_REFERENCE_VOLTAGE_VREF;
    dacParam.bufferSource = ECOMP_DAC_BUFFER_SOURCE_DUAL_BUFFER_1;
    dacParam.firstBufferData = 63;
    EComp_configureDAC(ECOMP0_BASE, &dacParam);
    EComp_enableDAC(ECOMP0_BASE);

    //Enable COMP_E Interrupt on default rising edge for CEIFG
    EComp_setInterruptEdgeDirection(ECOMP0_BASE, ECOMP_OUTPUT_INTERRUPT_RISING_EDGE);

    // Clear any erroneous interrupts
    EComp_clearInterrupt(ECOMP0_BASE, ECOMP_OUTPUT_INTERRUPT_FLAG |
        ECOMP_INVERTED_POLARITY_INTERRUPT_FLAG);

    //Enable Interrupts
    /*
     * Base Address of Comparator E,
     * Enable COMPE Interrupt on default rising edge for CPIFG
     */
    EComp_clearInterrupt(ECOMP0_BASE,
        ECOMP_OUTPUT_INTERRUPT_FLAG
        );

    EComp_enableInterrupt(ECOMP0_BASE,
        ECOMP_OUTPUT_INTERRUPT
        );

    //Allow power to Comparator module
    EComp_enable(ECOMP0_BASE);

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
#pragma vector=ECOMP0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(ECOMP0_VECTOR)))
#endif
void ECOMP_ISR (void)
{
    //Toggle the edge at which an interrupt is generated
    EComp_toggleInterruptEdgeDirection(ECOMP0_BASE);

    //Clear Interrupt flag
    EComp_clearInterrupt(ECOMP0_BASE, ECOMP_OUTPUT_INTERRUPT_FLAG);

	//Toggle P1.0 output pin
	GPIO_toggleOutputOnPin(
		GPIO_PORT_P1,
		GPIO_PIN0
	);
}

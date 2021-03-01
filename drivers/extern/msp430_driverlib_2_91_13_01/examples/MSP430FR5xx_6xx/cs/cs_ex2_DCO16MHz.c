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
//   MSP430FR59xx Demo - Configure MCLK for 16MHz operation
//
//   Description: Configure SMCLK = MCLK = 16MHz
//   IMPORTANT NOTE: While the FRxxx is capable of operation w/ MCLK = 16MHz
//   the throughput of the device is dependent on accesses to FRAM.
//   The maximum speed for accessing FRAM is limited to 8MHz*.
//   Wait states are inserted when the FRAM is accessed at
//   speeds higher than 8MHz to prevent timing violations.
//   Refer to the FRCTL chapter of the User's Guide.
//   * Refer device D/s for typical numbers
//
//           MSP430FR5969
//         ---------------
//     /|\|               |
//      | |               |
//      --|RST            |
//        |               |
//        |               |
//        |          P3.4 |---> SMCLK = MCLK = 16MHz
//
//******************************************************************************
#include "driverlib.h"

uint32_t clockValue;

void main(void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);
    //Set DCO frequency to 16MHz
    CS_setDCOFreq(CS_DCORSEL_1, CS_DCOFSEL_4);

    CS_initClockSignal(CS_SMCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);

    //Set wait state to 1
    FRAMCtl_configureWaitStateControl(FRAMCTL_ACCESS_TIME_CYCLES_1); 

    //Set P2.0 as Output Low (for setup as ACLK).
    /*

    * Select Port 2
    * Set Pin 0 to output Low.
    */
    GPIO_setOutputLowOnPin(
    	GPIO_PORT_P3,
    	GPIO_PIN4
    );

    //Set P2.0 as Ternary Module Function Output.
    /*

    * Select Port 2
    * Set Pin 0 to output Ternary Module Function, (ACLK).
    */
    GPIO_setAsPeripheralModuleFunctionOutputPin(
    	GPIO_PORT_P3,
    	GPIO_PIN4,
    	GPIO_TERNARY_MODULE_FUNCTION
    );

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    while(1);

}



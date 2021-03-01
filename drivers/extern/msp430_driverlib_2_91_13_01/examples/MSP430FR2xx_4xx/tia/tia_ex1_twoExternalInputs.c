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
//  MSP430FR231x Demo - TIA and feedback to comparator as input
//
//  Description: Configure TIA to take external postive/negative inputs
//  and output to the external pad. The power mode is selected
//  to low speed and low power.
//  ACLK = n/a, MCLK = SMCLK = default DCODIV ~1MHz.
//
//                MSP430FR231x
//             -------------------
//         /|\|                   |
//          | |         P1.7/TRI0+|<-- External pad
//          --|RST                |
//            |         P1.6/TRI0-|<-- External pad
//            |                   |
//            |         P1.5/TRI0O|--> External pad
//            |                   |
//            |                   |  
//            |                   |
//  Yang Lu
//  Texas Instruments Inc.
//  July 2015
//  Built with Code Composer Studio v6.1
//*****************************************************************************
#include "driverlib.h"

int main(void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    //Configure P1.2/P1.4 as OA0 functionality
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, 
      GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7,
      GPIO_TERNARY_MODULE_FUNCTION);
    PMM_unlockLPM5();                     // Disable the GPIO power-on default high-impedance mode
                                            // to activate 1previously configured port settings

    //Select external source as the positive input
    TIA_selectPositiveInput(TIA0_BASE, TIA_POSITIVE_INPUT_EXTERNAL_SOURCE);

    //Configure TIA low speed low power mode
    TIA_selectPowerMode(TIA0_BASE, TIA_LOW_SPEED_LOW_POWER);

    //Enable TIA
    TIA_enable(TIA0_BASE);
    
    //Enter LPM4 with interrupts enabled
    __bis_SR_register(LPM4_bits + GIE);

    //For debug
    __no_operation();
}

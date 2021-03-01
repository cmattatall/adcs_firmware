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
//! SAC - SAC-L3, Unity-Gain Buffer Mode
//!
//! Description: Configure SAC-L3 for Unity-Gain Buffer Mode. The "+" terminal
//! is connected to OA0+. Op-amp's negative input is connected with its
//! positive input inside the silicon. SAC is set to low speed and low power mode.
//! ACLK = n/a, MCLK = SMCLK = default DCODIV ~1MHz.
//!
//!           MSP430FR2xx_4xx Board
//!             -------------------
//!         /|\|               OA0-|
//!          | |                |  |
//!          | |               OA0O|
//!          --|RST                |
//!            |               OA0+|<--
//!            |                   |
//!            |                   |
//!
//! Wallace Tran
//! Texas Instruments Inc.
//! May 2018
//******************************************************************************
#include "Board.h"
#include "driverlib.h"

int main(void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    //Configure OA0 functionality
    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_SACOA0O,
            GPIO_PIN_SACOA0O,
            GPIO_FUNCTION_SACOA0O);
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_SACOA0P,
            GPIO_PIN_SACOA0P,
            GPIO_FUNCTION_SACOA0P);

    /*
     * Disable the GPIO power-on default high-impedance mode
     * to activate previously configured port settings
     */
    PMM_unlockLPM5();

    //Select external source for positive input
    //Select PGA source for negative input
    SAC_OA_init(
            SAC0_BASE,
            SAC_OA_POSITIVE_INPUT_SOURCE_EXTERNAL,
            SAC_OA_NEGATIVE_INPUT_SOURCE_PGA);

    //Select low speed and low power mode
    SAC_OA_selectPowerMode(
            SAC0_BASE,
            SAC_OA_POWER_MODE_LOW_SPEED_LOW_POWER);
    //Set as Unity-Gain Buffer Mode
    SAC_PGA_setMode(
            SAC0_BASE,
            SAC_PGA_MODE_BUFFER);
    //Enable OA
    SAC_OA_enable(SAC0_BASE); //Enable SAC
    SAC_enable(SAC0_BASE);

    //Enter LPM3
    __bis_SR_register(LPM3_bits);
    //For debug
    __no_operation();
}


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
//*****************************************************************************
//  MSP430FG662x Demo - OA0, General-Purpose Mode
//
//  Description: Configure OA0 for General-Purpose mode. The "+" terminal is
//  connected to OA0IP0 and the "-" terminal is connected to OA0IN0.
//  ACLK = n/a, MCLK = SMCLK = default DCO
//
//                MSP430FG662x
//             -------------------
//         /|\|                XIN|-
//          | |                   |
//          --|RST            XOUT|-
//            |                   |
//     "-" -->|P6.5/OA0IN0        |
//     "+" -->|P6.2/OA0IP0        |
//            |                   |
//            |          P6.4/OA0O|--> OA0 Output
//            |                   |
//            |                   |
//
//*****************************************************************************
#include <driverlib.h>

int main(void)
{
  // Stop watchdog timer
  WDT_A_hold(WDT_A_BASE);

  // Select P6.2,5 OA0IP/N0 function
  GPIO_setAsPeripheralModuleFunctionOutputPin(
  	        GPIO_PORT_P6,
  	        GPIO_PIN2 + GPIO_PIN5
  	        );


  // Select OA0IP0 as "+" input
  // Select OA0IN0 as "-" input
  OA_closeSwitch(OA_0_BASE,
  		  OA_POSITIVE_INPUT_TERMINAL_SWITCH0,
  		  OA_NEGATIVE_INPUT_TERMINAL_SWITCH0,
  		  OA_GROUND_NONE
  		  );

  // Enable OA0 amplifier
  OA_enableAmplifierMode(OA_0_BASE);

  // LPM3
  LPM3;
}

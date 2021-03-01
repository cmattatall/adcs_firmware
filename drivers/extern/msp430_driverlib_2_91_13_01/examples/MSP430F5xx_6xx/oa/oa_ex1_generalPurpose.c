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
//  MSP430FG662x Demo - OA0, Comparator in General-Purpose Mode
//
//  Description: Use OA0 as a comparator in general-purpose mode. In this
//  example, OA0 is configured for general-purpose mode, but used as a
//  comparator. The reference is supplied by DAC12_0 on the "+" input.
//  The "-" terminal is connected to OA0I0.
//  ACLK = n/a, MCLK = SMCLK = default DCO
//
//                MSP430FG662x
//             -------------------
//         /|\|                XIN|-
//          | |                   |
//          --|RST            XOUT|-
//            |                   |
//     "-" -->|P6.5/OA0IN0        |
//            |                   |
//            |          P6.4/OA0O|--> OA0 Output
//            |                   |    Output goes high when
//            |                   |    Input < .6V
//
//*****************************************************************************
#include <driverlib.h>

int main(void)
{
  // Stop watchdog timer
  WDT_A_hold(WDT_A_BASE);

  // Select P6.5 OA0IN0 function
  GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_P6,
            GPIO_PIN5
            );

  //AVcc reference for DAC12
  DAC12_A_initParam param = {0};

  param.submoduleSelect = DAC12_A_SUBMODULE_0;
  param.outputSelect = DAC12_A_OUTPUT_1;
  param.positiveReferenceVoltage = DAC12_A_VREF_AVCC;
  param.outputVoltageMultiplier = DAC12_A_VREFx1;
  param.amplifierSetting = DAC12_A_AMP_LOWIN_LOWOUT;
  param.conversionTriggerSelect = DAC12_A_TRIGGER_ENCBYPASS;

  DAC12_A_init(DAC12_A_BASE,&param);
  DAC12_A_enableConversions(DAC12_A_BASE, DAC12_A_SUBMODULE_0);
  DAC12_A_setData(DAC12_A_BASE, DAC12_A_SUBMODULE_0, 0x7FF);

  // Reference level = 1.5V
  // Select DAC12_0 as "+" input
  // Select OA0IN0 as "-" input
  OA_closeSwitch(OA_0_BASE,
      OA_POSITIVE_INPUT_TERMINAL_SWITCH1,
      OA_NEGATIVE_INPUT_TERMINAL_SWITCH0,
      OA_GROUND_NONE
      );

  // Enable OA0 amplifier
  OA_enableAmplifierMode(OA_0_BASE);

  LPM3;                                     // LPM3
}

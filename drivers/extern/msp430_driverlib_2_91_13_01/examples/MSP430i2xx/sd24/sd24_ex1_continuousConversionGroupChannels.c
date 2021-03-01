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
//  MSP430i20xx SD24 Example 01 - SD24, Continuous Conversion on a Group of 3 Channels
//
//  Description: This program uses the SD24 module to perform continuous
//  conversions on a group of channels (0, 1 and 2). A SD24 interrupt occurs
//  whenever the conversions have completed.
//
//  Test by applying voltages to the 3 input channels and setting a breakpoint
//  at the indicated line. Run program until it reaches the breakpoint, then use
//  the debugger's watch window to view the conversion results.
//
//  Results (upper 16 bits only) are stored in three arrays, one for each channel.
//
//  ACLK = 32kHz, MCLK = SMCLK = Calibrated DCO = 16.384MHz, SD_CLK = 1.024MHz
//  * Ensure low_level_init.c is included when building/running this example *
//
//  Notes: For minimum Vcc required for SD24 module - see datasheet
//         100nF cap btw Vref and AVss is recommended when using 1.2V ref
//
//               MSP430i20xx
//             -----------------
//         /|\|                |
//          | |                |
//          --|RST             |
//            |                |
//   Vin1+ -->|A0.0+      VREF |---+
//   Vin1- -->|A0.0-           |   |
//   Vin2+ -->|A1.0+           |  -+- 100nF
//   Vin2- -->|A1.0-           |  -+-
//   Vin3+ -->|A2.0+           |   |
//   Vin3- -->|A2.0-      AVss |---+
//
//  T. Witt
//  Yang Lu
//  Texas Instruments, Inc
//  June 2014
//  Built with Code Composer Studio v5.5
//******************************************************************************
#include "driverlib.h"

#define Num_of_Results   8

/* Arrays to store SD24 conversion results */
uint16_t Ch0results[Num_of_Results];
uint16_t Ch1results[Num_of_Results];
uint16_t Ch2results[Num_of_Results];
uint16_t i = 0;

void main(void) {
    // Stop WDT
    WDT_hold(WDT_BASE);                            

    // Internal ref
    SD24_init(SD24_BASE, SD24_REF_INTERNAL);       

    //Group with Channel 0
    SD24_initConverterAdvancedParam param = {0};
    param.converter = SD24_CONVERTER_0;
    param.conversionMode = SD24_CONTINUOUS_MODE;
    param.groupEnable = SD24_GROUPED;
    param.inputChannel = SD24_INPUT_CH_ANALOG;
    param.dataFormat = SD24_DATA_FORMAT_2COMPLEMENT;
    param.interruptDelay = SD24_FOURTH_SAMPLE_INTERRUPT;
    param.oversampleRatio = SD24_OVERSAMPLE_256;
    param.gain = SD24_GAIN_1;
    SD24_initConverterAdvanced(SD24_BASE, &param);

    //Group with Channel 1
    param.converter = SD24_CONVERTER_1;
    param.conversionMode = SD24_CONTINUOUS_MODE;
    param.groupEnable = SD24_GROUPED;
    param.inputChannel = SD24_INPUT_CH_ANALOG;
    param.dataFormat = SD24_DATA_FORMAT_2COMPLEMENT;
    param.interruptDelay = SD24_FOURTH_SAMPLE_INTERRUPT;
    param.oversampleRatio = SD24_OVERSAMPLE_256;
    param.gain = SD24_GAIN_1;
    SD24_initConverterAdvanced(SD24_BASE, &param);

    // Enable interrupt
    param.converter = SD24_CONVERTER_2;
    param.conversionMode = SD24_CONTINUOUS_MODE;
    param.groupEnable = SD24_NOT_GROUPED;
    param.inputChannel = SD24_INPUT_CH_ANALOG;
    param.dataFormat = SD24_DATA_FORMAT_2COMPLEMENT;
    param.interruptDelay = SD24_FOURTH_SAMPLE_INTERRUPT;
    param.oversampleRatio = SD24_OVERSAMPLE_256;
    param.gain = SD24_GAIN_1;
    SD24_initConverterAdvanced(SD24_BASE, &param);
    SD24_enableInterrupt(SD24_BASE, SD24_CONVERTER_2, SD24_CONVERTER_INTERRUPT);

    // Delay ~200us for 1.2V ref to settle
    __delay_cycles(3200);

    // Start conversion
    SD24_startConverterConversion(SD24_BASE, SD24_CONVERTER_2);
    // Enter LPM0 w/ interrupts
    __bis_SR_register(LPM0_bits | GIE);
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=SD24_VECTOR
__interrupt void SD24_ISR(void) {
#elif defined(__GNUC__)
void __attribute__ ((interrupt(SD24_VECTOR))) SD24_ISR (void) {
#else
#error Compiler not supported!
#endif
    switch (__even_in_range(SD24IV,SD24IV_SD24MEM3)) {
        case SD24IV_NONE: break;
        case SD24IV_SD24OVIFG: break;
        case SD24IV_SD24MEM0: break;
        case SD24IV_SD24MEM1: break;
        case SD24IV_SD24MEM2:
                   // Save CH0 results (clears IFG)
                   Ch0results[i] = SD24_getHighWordResults(SD24_BASE, SD24_CONVERTER_0);
                   // Save CH1 results (clears IFG)
                   Ch1results[i] = SD24_getHighWordResults(SD24_BASE, SD24_CONVERTER_1);
                   // Save CH2 results (clears IFG)
                   Ch2results[i] = SD24_getHighWordResults(SD24_BASE, SD24_CONVERTER_2); 
                   i++;
                   if (i == Num_of_Results) {
                       i = 0;
                       __no_operation();           // SET BREAKPOINT HERE
                   }
                   break;
        case SD24IV_SD24MEM3: break;
        default: break;
    }
}

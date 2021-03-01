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
//  MSP430i20xx SD24 Example 04 - SD24, Single Conversion on a Single Channel, Poll IFG
//
//  Description: This program uses the SD24 module to perform a single
//  conversion on a single channel. The SD24 interrupt flag for CH0 is polled
//  to determine when a conversion has completed.
//
//  Test by applying a voltage to the input channel and setting a breakpoint
//  at the indicated line. Run program until it reaches the breakpoint, then use
//  the debugger's watch window to view the conversion results.
//
//  Results (upper 16 bits only) are stored in the variable "results"
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
//            |                |  -+- 100nF
//            |                |  -+-
//            |                |   |
//            |           AVss |---+
//
//  T. Witt
//  Yang Lu
//  Texas Instruments, Inc
//  June 2014
//  Built with Code Composer Studio v5.5
//******************************************************************************
#include "driverlib.h"

uint16_t results;                       // SD24 Conversion Results

void main(void) {
    // Stop WDT
    WDT_hold(WDT_BASE); 

    // Internal ref
    SD24_init(SD24_BASE, SD24_REF_INTERNAL);

    //Ch0 single mode
    SD24_initConverterAdvancedParam param = {0};
    param.converter = SD24_CONVERTER_0;
    param.conversionMode = SD24_SINGLE_MODE;
    param.groupEnable = SD24_NOT_GROUPED;
    param.inputChannel = SD24_INPUT_CH_ANALOG;
    param.dataFormat = SD24_DATA_FORMAT_2COMPLEMENT;
    param.interruptDelay = SD24_FOURTH_SAMPLE_INTERRUPT;
    param.oversampleRatio = SD24_OVERSAMPLE_256;
    param.gain = SD24_GAIN_1;
    SD24_initConverterAdvanced(SD24_BASE, &param);

    // Delay ~200us for 1.2V ref to settle
    __delay_cycles(3200);

    while(1) {
        // Start conversion
        SD24_startConverterConversion(SD24_BASE, SD24_CONVERTER_0);
        // Poll IFG until conversion completes
        while(!SD24_getInterruptStatus(SD24_BASE, SD24_CONVERTER_0, SD24_CONVERTER_INTERRUPT));
        // Save SD24 conversion results
        results = SD24_getHighWordResults(SD24_BASE, SD24_CONVERTER_0);
        __no_operation();                   // SET BREAKPOINT HERE
    }
}

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
#include "driverlib.h"

//*****************************************************************************
//  MSP430F673x Demo - SD24_B, Single Conversion on Single Channel Polling IFG
//
//  Description: This program uses the SD24_B module to perform a single
//  conversion on a single channel. The SD24_B interrupt flag for channel 2 is
//  polled to determine when a conversion has completed. Test by applying a
//  voltage to channel 2 (SD2P0, SD2N0) and setting a breakpoint at the line
//  indicated below. Run program until it reaches the breakpoint, then use the
//  debugger's watch window to view the conversion results. Results (upper 16
//  bits only) for channel 2 are stored in variable "results".
//  ACLK = n/a, MCLK = SMCLK = DCO =  ~ 1.1MHz
//  //* For minimum Vcc required for SD24_B module - see datasheet          *//
//  //* 100nF cap between Vref and AVss is recommended when using 1.5V REF  *//
//
//                MSP430F673x
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//    Vin+ -->|SD2P0            |
//    Vin- -->|SD2N0            |
//            |                 |
//            |            VREF |---+
//            |                 |   |
//            |                 |  -+- 100nF
//            |                 |  -+-
//            |                 |   |
//            |            AVss |---+
//            |                 |
//
//  M. Swanson
//  Texas Instruments, Inc
//  December 2011
//  Built with CCS Version: 5.1.0 and IAR Embedded Workbench Version: 5.40.1
//*****************************************************************************

/* uint32_t to store SD24_B conversion result */
uint32_t results;

void main(void)
{
	WDT_A_hold(WDT_A_BASE); // Stop WDT

	// Select internal REF
    // Select SMCLK as SD24_B clock source
	SD24_B_initParam initParam = {0};
    initParam.clockSourceSelect = SD24_B_CLOCKSOURCE_SMCLK;
    initParam.clockPreDivider = SD24_B_PRECLOCKDIVIDER_1;
    initParam.clockDivider = SD24_B_CLOCKDIVIDER_1;
    initParam.referenceSelect = SD24_B_REF_INTERNAL;
    SD24_B_init(SD24_BASE, &initParam);
	
	SD24_B_initConverterParam initConverterParam = {0};
    initConverterParam.converter = SD24_B_CONVERTER_2;
    initConverterParam.alignment = SD24_B_ALIGN_RIGHT;
    initConverterParam.startSelect = SD24_B_CONVERSION_SELECT_SD24SC;
    initConverterParam.conversionMode = SD24_B_SINGLE_MODE;
    SD24_B_initConverter(SD24_BASE, &initConverterParam);

    __delay_cycles(0x3600);                   // Delay for 1.5V REF startup

    while (1)
    {
        SD24_B_startConverterConversion(SD24_BASE,
          	SD24_B_CONVERTER_2);								 // Set bit to start conversion

        // Poll interrupt flag for channel 2
        while( SD24_B_getInterruptStatus(SD24_BASE,
        	SD24_B_CONVERTER_2,
        	SD24_B_CONVERTER_INTERRUPT) == 0 );

        results = SD24_B_getResults(SD24_BASE,
           	SD24_B_CONVERTER_2);                			// Save CH2 results (clears IFG)

        __no_operation();                     // SET BREAKPOINT HERE
    }
}


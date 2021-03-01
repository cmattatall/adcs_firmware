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
//  MSP430F673x Demo - SD24_B, Single Conversion on a Group of 3 Channels
//
//  Description: This program uses the SD24_B module to perform a single
//  conversion on a group of 3 channels (0, 1 and 2). An SD24_B interrupt
//  occurs when the conversions have completed. Test by applying voltages
//  to the 3 input channels and setting a breakpoint at the line indicated
//  below. Run program until it reaches the breakpoint, then use the
//  debugger's watch window to view the conversion results. Results (upper 16
//  bits only) for all 3 channels are stored in the array "results".
//  ACLK = n/a, MCLK = SMCLK = DCO =  ~ 1.1MHz
//  //* For minimum Vcc required for SD24_B module - see datasheet          *//
//  //* 100nF cap between Vref and AVss is recommended when using 1.5V REF  *//
//
//                MSP430F673x
//            -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//   Vin1+ -->|SD0P0            |
//   Vin1- -->|SD0N0            |
//   Vin2+ -->|SD1P0            |
//   Vin2- -->|SD1N0            |
//   Vin3+ -->|SD2P0            |
//   Vin3- -->|SD2N0            |
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

/* Array to store SD24_B conversion results */
uint32_t results[3];

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
	
	SD24_B_initConverterParam initConverterParam0 = {0};
    initConverterParam0.converter = SD24_B_CONVERTER_0;
    initConverterParam0.alignment = SD24_B_ALIGN_RIGHT;
    initConverterParam0.startSelect = SD24_B_CONVERSION_SELECT_GROUP1;
    initConverterParam0.conversionMode = SD24_B_CONTINUOUS_MODE;
    SD24_B_initConverter(SD24_BASE, &initConverterParam0);
	
	SD24_B_initConverterParam initConverterParam1 = {0};
    initConverterParam1.converter = SD24_B_CONVERTER_1;
    initConverterParam1.alignment = SD24_B_ALIGN_RIGHT;
    initConverterParam1.startSelect = SD24_B_CONVERSION_SELECT_GROUP1;
    initConverterParam1.conversionMode = SD24_B_CONTINUOUS_MODE;
    SD24_B_initConverter(SD24_BASE, &initConverterParam1);
	
	SD24_B_initConverterParam initConverterParam2 = {0};
    initConverterParam2.converter = SD24_B_CONVERTER_2;
    initConverterParam2.alignment = SD24_B_ALIGN_RIGHT;
    initConverterParam2.startSelect = SD24_B_CONVERSION_SELECT_GROUP1;
    initConverterParam2.conversionMode = SD24_B_CONTINUOUS_MODE;
    SD24_B_initConverter(SD24_BASE, &initConverterParam2);

	SD24_B_clearInterrupt(SD24_BASE,
	 		SD24_B_CONVERTER_2,
	 		SD24_B_CONVERTER_INTERRUPT	);                   // Clear channel 2 interrupt
	 SD24_B_enableInterrupt(SD24_BASE,
	 		SD24_B_CONVERTER_2,
	 		SD24_B_CONVERTER_INTERRUPT	);                   // Enable channel 2 interrupt

    __delay_cycles(0x3600);                 // Delay for 1.5V REF startup

    while (1)
    {

    	SD24_B_startGroupConversion(SD24_BASE,
    	    SD24_B_CONVERTER_1);								// Set bit to start conversion

    	__bis_SR_register(LPM0_bits | GIE); // Enter LPM0 w/ interrupts

    	SD24_B_stopGroupConversion(SD24_BASE,
    		SD24_B_CONVERTER_1);	  			      			// Clear bit for next conversion

        __no_operation();                   // SET BREAKPOINT HERE
    }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=SD24B_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(SD24B_VECTOR)))
#endif
void SD24BISR(void)
{
    switch (SD24BIV)
    {
        case SD24BIV_SD24OVIFG:             // SD24MEM Overflow
            break;
        case SD24BIV_SD24TRGIFG:            // SD24 Trigger IFG
            break;
        case SD24BIV_SD24IFG0:              // SD24MEM0 IFG
            break;
        case SD24BIV_SD24IFG1:              // SD24MEM1 IFG
            break;
        case SD24BIV_SD24IFG2:              // SD24MEM2 IFG
            results[0] = SD24_B_getResults(SD24_BASE,
            	SD24_B_CONVERTER_0);							// Save CH0 results (clears IFG)
            results[1] = SD24_B_getResults(SD24_BASE,
                SD24_B_CONVERTER_1);							// Save CH1 results (clears IFG)
            results[2] = SD24_B_getResults(SD24_BASE,
            	SD24_B_CONVERTER_2);							// Save CH2 results (clears IFG)
            break;
    }

    __bic_SR_register_on_exit(LPM0_bits);   // Exit LPM0
}


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
//  MSP430FG662x Demo - CTSD16, Continuous Rail-to-Rail Conversion, Single Channel
//
//  Description: This program uses the CTSD16 module to perform continuous
//  conversions on a single channel with rail-to-rail input. A CTSD16 interrupt
//  occurs when a conversion has completed.
//
//  Test by applying voltages to the input channel and setting a breakpoint
//  at the indicated line. Run program until it reaches the breakpoint, then use
//  the debugger's watch window to view the conversion results.
//
//  Results (upper 16 bits only) are stored in the array "results"
//
//  ACLK = 32kHz, MCLK = SMCLK = Calibrated DCO = 16.384MHz, SD_CLK = 1.024MHz
//
//  Notes: For minimum Vcc required for CTSD16 module - see datasheet
//         1nF cap btw Vref and AVss is recommended when using 1.2V ref
//
//               MSP430FG662x
//             -----------------
//         /|\|                |
//          | |                |
//          --|RST             |
//            |                |
//   Vin1+ -->|A0.0+      VREF |---+
//   Vin1- -->|A0.0-           |   |
//            |                |  -+- 1nF
//            |                |  -+-
//            |                |   |
//            |           AVss |---+
//
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - CTSD16 peripheral
//! - GPIO Port peripheral
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - CTSD16_VECTOR
//******************************************************************************

#include "driverlib.h"

uint32_t results[8];

void main(void) {
    // Stop WDT
    WDT_A_hold(WDT_A_BASE);

    // Select AD0+/- analog input pins
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN4 | GPIO_PIN5);

    // Initialize CTSD16 using internal reference and internal resistor for clock
    CTSD16_init(CTSD16_BASE,
            CTSD16_RTR_INPUT_CHARGEPUMP_BURST_REQUEST_DISABLE, CTSD16_REF_INTERNAL);

    // Initialize converter 0: AD0+ / AD0- as input, 2s complement, channel 9
    CTSD16_initConverterAdvancedParam convAdvParam = {0};
    convAdvParam.converter = CTSD16_CONVERTER_0;
    convAdvParam.conversionMode = CTSD16_CONTINUOUS_MODE;
    convAdvParam.groupEnable = CTSD16_NOT_GROUPED;
    convAdvParam.inputChannel = CTSD16_INPUT_CH9;
    convAdvParam.dataFormat = CTSD16_DATA_FORMAT_2COMPLEMENT;
    convAdvParam.railToRailInput = CTSD16_RTR_INPUT_ENABLE;
    convAdvParam.interruptDelay = CTSD16_FOURTH_SAMPLE_INTERRUPT;
    convAdvParam.oversampleRatio = CTSD16_OVERSAMPLE_256;
    convAdvParam.gain = CTSD16_GAIN_1;
    CTSD16_initConverterAdvanced(CTSD16_BASE, &convAdvParam);

    // Clear converter 0 interrupt flags
    CTSD16_clearInterrupt(CTSD16_BASE, CTSD16_CONVERTER_0,
            CTSD16_CONVERTER_INTERRUPT|CTSD16_CONVERTER_OVERFLOW_INTERRUPT);
    // Enable converter 0 overflow, result interrupts
    CTSD16_enableInterrupt(CTSD16_BASE, CTSD16_CONVERTER_0,
            CTSD16_CONVERTER_INTERRUPT|CTSD16_CONVERTER_OVERFLOW_INTERRUPT);

    // Delay ~120us for 1.2V ref to settle
    __delay_cycles(2000);

    // Wait for rail-to-rail input ready
    while(!CTSD16_isRailToRailInputReady(CTSD16_BASE));

    // Set bit to start conversion
    CTSD16_startConverterConversion(CTSD16_BASE, CTSD16_CONVERTER_0);

    __bis_SR_register(LPM0_bits | GIE);     // Enter LPM0 w/ interrupts
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=CTSD16_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(CTSD16_VECTOR)))
#endif
void CTSD16_ISR(void) {
    static uint8_t index = 0;
    switch (__even_in_range(CTSD16IV,CTSD16IV_CTSD16MEM0)) {
        case CTSD16IV_NONE: break;
        case CTSD16IV_CTSD16OVIFG:
            // Clear overflow interrupt explicitly
            CTSD16_clearInterrupt(CTSD16_BASE, CTSD16_CONVERTER_0, CTSD16_CONVERTER_OVERFLOW_INTERRUPT);
            break;
        case CTSD16IV_CTSD16MEM0:
            // Save CH0 results (clears IFG)
            results[index++] = CTSD16_getResults(CTSD16_BASE, CTSD16_CONVERTER_0);
            if(index >= 8) {
               index = 0;                 // SET BREAKPOINT HERE
            }
            break;
        default: break;
    }
}

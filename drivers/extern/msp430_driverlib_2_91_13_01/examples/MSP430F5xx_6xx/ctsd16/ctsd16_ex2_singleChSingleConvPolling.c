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
//  MSP430FG662x Demo - CTSD16, Single Conversion on a Single Channel, Poll IFG
//
//  Description: This program uses the CTSD16 module to perform a single
//  conversion on a single channel. The CTSD16 interrupt flag for CH0 is polled
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
//  Notes: For minimum Vcc required for CTSD16 module - see datasheet
//         1nF cap btw Vref and AVss is recommended when using 1.2V ref
//
//               MSP430FG662x
//             -----------------
//         /|\|                |
//          | |                |
//          --|RST             |
//            |                |
//   Vin1+ -->|P6.4/A0.0+ VREF |---+
//   Vin1- -->|P6.5/A0.0-      |   |
//            |                |  -+- 1nF
//            |                |  -+-
//            |                |   |
//            |           AVss |---+
//
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
#include "inc/hw_memmap.h"
#include "wdt_a.h"
#include "gpio.h"
#include "ctsd16.h"

uint32_t  results;                          // CTSD16 Conversion Results

void main(void) {
    // Stop WDT
    WDT_A_hold(WDT_A_BASE);

    // Select AD0+/- analog input pins
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN4 | GPIO_PIN5);

    // Initialize CTSD16 using internal reference and internal resistor for clock
    CTSD16_init(CTSD16_BASE,
            CTSD16_RTR_INPUT_CHARGEPUMP_BURST_REQUEST_DISABLE, CTSD16_REF_INTERNAL);

    // Initialize converter 0: AD0+ / AD0- as input, channel 9
    CTSD16_initConverter(CTSD16_BASE, CTSD16_CONVERTER_0, CTSD16_SINGLE_MODE, 
        CTSD16_INPUT_CH9);

    // Delay ~120us for 1.2V ref to settle
    __delay_cycles(2000);

    while(1) {
         // Set bit to start conversion
        CTSD16_startConverterConversion(CTSD16_BASE, CTSD16_CONVERTER_0);
        // Poll IFG until conversion completes
        while(!CTSD16_getInterruptStatus(CTSD16_BASE, CTSD16_CONVERTER_0, CTSD16_CONVERTER_INTERRUPT));
        // Save CTSD16 conversion results
        results = CTSD16_getResults(CTSD16_BASE, CTSD16_CONVERTER_0);
        __no_operation();                       // SET BREAKPOINT HERE
    }
}

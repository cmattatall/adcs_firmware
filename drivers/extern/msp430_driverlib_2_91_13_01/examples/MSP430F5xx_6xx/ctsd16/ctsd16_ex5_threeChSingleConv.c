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
//  MSP430FG662x Demo - CTSD16, Single Conversion of 3 Input Channels
//
//  Description: This program uses the CTSD16 module to perform a single
//  conversion on 3 input channels (0, 1, and 2). A CTSD16 interrupt occurs when
//  the conversions have completed.
//
//  Test by applying voltages to the input channel and setting a breakpoint
//  at the indicated line. Run program until it reaches the breakpoint, then use
//  the debugger's watch window to view the conversion result.
//
//  Results (upper 16 bits only) are stored in the arrays "Chresults", with the
//  input channel number equal to the value's index (ex. input channel 0 stored
//  at Chresults[0]).
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
//   Vin1+ -->|P6.4/AD0+  VREF |---+
//   Vin1- -->|P6.5/AD0-       |   |
//   Vin2+ -->|P6.6/AD0+       |  -+- 1nF
//   Vin2- -->|P6.7/AD0-       |  -+-
//   Vin3+ -->|P7.4/AD0+       |   |
//   Vin3- -->|P7.5/AD0-  AVss |---+
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

#define Num_of_Channels    3

/* Arrays to store CTSD16 conversion results */
uint32_t Chresults[Num_of_Channels];
uint8_t i = 0;
uint8_t channels[Num_of_Channels] = {CTSD16_INPUT_CH9, CTSD16_INPUT_CH11, CTSD16_INPUT_CH13};

void main(void) {
    // Stop WDT
    WDT_A_hold(WDT_A_BASE);

    // Select AD0+/- analog input pins
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN4 | GPIO_PIN5 |
            GPIO_PIN6 | GPIO_PIN7);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P7, GPIO_PIN4 | GPIO_PIN5);

    // Initialize CTSD16 using internal reference and internal resistor for clock
    CTSD16_init(CTSD16_BASE,
            CTSD16_RTR_INPUT_CHARGEPUMP_BURST_REQUEST_DISABLE, CTSD16_REF_INTERNAL);

    // Initialize converter 0: AD0+ / AD0- as input, channel 9
    CTSD16_initConverter(CTSD16_BASE, CTSD16_CONVERTER_0, CTSD16_SINGLE_MODE, 
        CTSD16_INPUT_CH9);

    // Clear converter 0 interrupt flags
    CTSD16_clearInterrupt(CTSD16_BASE, CTSD16_CONVERTER_0,
            CTSD16_CONVERTER_INTERRUPT);
    // Enable result interrupts
    CTSD16_enableInterrupt(CTSD16_BASE, CTSD16_CONVERTER_0,
            CTSD16_CONVERTER_INTERRUPT);

    // Delay ~120us for 1.2V ref to settle
    __delay_cycles(2000);

    while(1) {
        // Set bit to start conversion
        CTSD16_startConverterConversion(CTSD16_BASE, CTSD16_CONVERTER_0);
        __bis_SR_register(LPM0_bits | GIE); // Enter LPM0 w/ interrupts
        __no_operation();                   // For debugger

        if (i >= Num_of_Channels) {
            i = 0;
        }
        CTSD16_setInputChannel(CTSD16_BASE, CTSD16_CONVERTER_0, channels[i]);
    }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=CTSD16_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(CTSD16_VECTOR)))
#endif
void CTSD16_ISR(void) {
    switch (__even_in_range(CTSD16IV,CTSD16IV_CTSD16MEM0)) {
        case CTSD16IV_NONE: break;
        case CTSD16IV_CTSD16OVIFG: break;
        case CTSD16IV_CTSD16MEM0:
            // Save CH0 results (clears IFG)
            Chresults[i++] = CTSD16_getResults(CTSD16_BASE, CTSD16_CONVERTER_0);
            __bic_SR_register_on_exit(LPM0_bits); // Wake up
            break;
        default: break;
    }
}

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
//  MSP430FG662x Demo - CTSD16, Using the Integrated Temperature Sensor, Ext. Res.
//
//  Description: This program uses the CTSD16 module to perform a single
//  conversion on a single channel which is internally connected to the CTSD16's
//  temperature sensor and an external resistor feeding the CTSD16 clock. Once
//  the conversion is completed, the result is stored in a variable then converted
//  into Celsius and Fahrenheit values.
//
//  Test by setting a breakpoint at the indicated line. Upon reaching the breakpoint
//  the conversion result will be stored in the results array. The result will
//  then be taken and converted into degrees K, C, and F and be saved in the same
//  array.
//
//  ACLK = 32kHz, MCLK = SMCLK = Calibrated DCO = 16.384MHz, SD_CLK = 1.024MHz
//  * Ensure low_level_init.c is included when building/running this example *
//
//  Notes: For minimum Vcc required for CTSD16 module - see datasheet
//         1nF cap btw Vref and AVss is recommended when using 1.2V ref
//
//  Sensor's temperature coefficient is 2.158mV/K (from datasheet)
//  Sensor's offset voltage ranges from -100mv to 100mV (assume 0)
//  Vsensor = 1.32mV * DegK + Vsensor_offset (assuming 0mv for offset)
//  Vsensor = (CTSD16MEM0)/32767 * Vref(mV)
//  DegK = (CTSD16MEM0 * 1200)/32767/2.158 = (CTSD16MEM0 * 1200)/70711
//  DegC =  DegK - 273
//  DegF = (DegC * 9/5) + 32
//
//
//               MSP430FG662x
//             -----------------
//         /|\|                |
//          | |                |
//          --|RST             |
//            |                |        (A0.6+/- connected internally)
//            |A0.6+      VREF |---+    (to CTSD16's temperature sensor)
//            |A0.6-           |   |
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
#include "inc/hw_memmap.h"
#include "wdt_a.h"
#include "gpio.h"
#include "ctsd16.h"

uint32_t results[4];        // CTSD16 Conversion and Temp Results
                            // results[0] = raw CTSD16 results
                            // results[1] = temp in K
                            // results[2] = temp in C
                            // results[3] = temp in F

void main(void) {
    // Stop WDT
    WDT_A_hold(WDT_A_BASE);

    // Select AD0+/- analog input pins
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN4 | GPIO_PIN5);

    // Initialize CTSD16 using internal reference and internal resistor for clock
    CTSD16_init(CTSD16_BASE,
            CTSD16_RTR_INPUT_CHARGEPUMP_BURST_REQUEST_DISABLE, CTSD16_REF_INTERNAL);

    // Initialize converter 0: internal temperature sensor channel 6
    CTSD16_initConverter(CTSD16_BASE, CTSD16_CONVERTER_0, CTSD16_SINGLE_MODE, 
        CTSD16_INPUT_CH6);

    // Clear converter 0 interrupt flag
    CTSD16_clearInterrupt(CTSD16_BASE, CTSD16_CONVERTER_0,
            CTSD16_CONVERTER_INTERRUPT);
    // Enable result interrupt
    CTSD16_enableInterrupt(CTSD16_BASE, CTSD16_CONVERTER_0,
            CTSD16_CONVERTER_INTERRUPT);

    // Delay ~120us for 1.2V ref to settle
    __delay_cycles(2000);

    while(1) {
        __no_operation();                   // SET BREAKPOINT HERE
        // Set bit to start conversion
        CTSD16_startConverterConversion(CTSD16_BASE, CTSD16_CONVERTER_0);
        __bis_SR_register(LPM0_bits | GIE); // Enter LPM0 w/ interrupts
        __no_operation();                   // For debugger

        // Calculate Temperatures in different scales
        results[1] = ((unsigned long)results[0] * 1200)/70711;
        results[2] = results[1] - 273;
        results[3] = (results[2] * 9/5) + 32;

        __no_operation();                   // SET BREAKPOINT HERE
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
            results[0] = CTSD16_getResults(CTSD16_BASE, CTSD16_CONVERTER_0);
            break;
        default: break;
    }
    __bic_SR_register_on_exit(LPM0_bits);       // Wake up from LPM0
}


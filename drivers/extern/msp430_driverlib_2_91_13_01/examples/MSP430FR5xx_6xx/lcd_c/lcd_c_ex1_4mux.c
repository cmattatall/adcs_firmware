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
//! LCD_C, 4-Mux example, display a string "123456" on LCD if breakpoint.
//! Displays "Ue9A68" if program is run all the way.
//! This examples shows how to config LCD in 4-Mux mode with DriverLib
//! functions. The internal voltage is sourced to v2 through v4 and v5 is
//! connected to ground. Charge pump is enabled.
//!
//! It uses LCD pin L0~L21 and L26~L43 as segment pins.
//! f(LCD) = 32768Hz/((1+1)*2^4) = 1024Hz, ACLK = ~32768Hz,
//! MCLK = SMCLK = default DCODIV ~1MHz.
//!
//!				MSP-EXP430FR6989
//!             -----------------
//!         /|\|                 |
//!          | |              XIN|--
//! GND      --|RST              |  ~32768Hz
//!  |         |             XOUT|--
//!  |
//!  |         |         	 COM3|----------------|
//!  |         |             COM2|---------------||
//!  |--4.7uF -|LCDCAP       COM1|--------------|||
//!            |             COM0|-------------||||
//!            |                 |    -------------
//!            |           L0~L21|---| 1 2 3 4 5 6 |
//!            |          L26~L43|    -------------
//!            |                 |       TI LCD
//!
//!
//! This example uses the following peripherals and I/O signals on 
//! MSP-EXP430FR6989. You must review these and change as needed 
//! for your own board:
//! - LCD
//! - GPIO Port peripheral (for LCD pins)
//!
//!
//*****************************************************************************
#include "driverlib.h"

int main(void) {

	WDT_A_hold(WDT_A_BASE);

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
	PMM_unlockLPM5();

    //enable interrupts
    __enable_interrupt();

	LCD_C_setPinAsLCDFunctionEx(LCD_C_BASE, LCD_C_SEGMENT_LINE_0, LCD_C_SEGMENT_LINE_21);
	LCD_C_setPinAsLCDFunctionEx(LCD_C_BASE, LCD_C_SEGMENT_LINE_26, LCD_C_SEGMENT_LINE_43);

	LCD_C_initParam initParams = {0};
	initParams.clockSource = LCD_C_CLOCKSOURCE_ACLK;
	initParams.clockDivider = LCD_C_CLOCKDIVIDER_1;
	initParams.clockPrescalar = LCD_C_CLOCKPRESCALAR_16;
	initParams.muxRate = LCD_C_4_MUX;
	initParams.waveforms = LCD_C_LOW_POWER_WAVEFORMS;
	initParams.segments = LCD_C_SEGMENTS_ENABLED;

	LCD_C_init(LCD_C_BASE, &initParams);

	// LCD Operation - VLCD generated internally, V2-V4 generated internally, v5 to ground
	LCD_C_setVLCDSource(LCD_C_BASE, LCD_C_VLCD_GENERATED_INTERNALLY, LCD_C_V2V3V4_GENERATED_INTERNALLY_NOT_SWITCHED_TO_PINS,
		 LCD_C_V5_VSS);

	// Set VLCD voltage to 2.60v
	LCD_C_setVLCDVoltage(LCD_C_BASE, LCD_C_CHARGEPUMP_VOLTAGE_2_60V_OR_2_17VREF);

	// Enable charge pump and select internal reference for it
	LCD_C_enableChargePump(LCD_C_BASE);
	LCD_C_selectChargePumpReference(LCD_C_BASE, LCD_C_INTERNAL_REFERENCE_VOLTAGE);

	LCD_C_configChargePump(LCD_C_BASE, LCD_C_SYNCHRONIZATION_ENABLED, 0);

	// Clear LCD memory
	LCD_C_clearMemory(LCD_C_BASE);

    // Display "123456"
	// LCD Pin18-Pin19 for '1'
	LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_18, 0x0);
	LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_19, 0x6);

	// LCD Pin10-Pin11 for '2'
	LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_10, 0xB);
	LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_11, 0xD);

    // LCD Pin6-Pin7 for '3'
	LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_6, 0x3);
	LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_7, 0xF);

    // LCD Pin36-Pin37 for '4'
	LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_36, 0x7);
	LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_37, 0x6);

    // LCD Pin28-Pin29 for '5'
	LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_28, 0x7);
	LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_29, 0xB);

    // LCD Pin14-Pin15 for '6'
	LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_14, 0xF);
	LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_15, 0xB);

	// Turn LCD on
	LCD_C_on(LCD_C_BASE);
    // Set breakpoint here to see the "123456"
    __no_operation();
    // Prepare LCD for adding new segments to old letters to make new letters
    LCD_C_off(LCD_C_BASE);
    /**
     * Rough picture representation of hex value representation to physical lines on a LCD_C
     * lower    upper
     * line     line
     * segment  segment
     *    \ 0x8
     * 0x4 |____0x4
     *   0x2  0x1 |
     * 0x8 ___/ 0x2
     *    / 0x1
     */
    // Display "Ue9A68"
    // LCD Pin18-Pin19 for changing '1' to 'U'
    LCD_C_setMemoryWithoutOverwrite(LCD_C_BASE, LCD_C_SEGMENT_LINE_18, 0xC);
    LCD_C_setMemoryWithoutOverwrite(LCD_C_BASE, LCD_C_SEGMENT_LINE_19, 0x1);

    // LCD Pin10-Pin11 for changing '2' to 'e'
    LCD_C_setMemoryWithoutOverwrite(LCD_C_BASE, LCD_C_SEGMENT_LINE_10, 0x4);

    // LCD Pin6-Pin7 for changing '3' to '9'
    LCD_C_setMemoryWithoutOverwrite(LCD_C_BASE, LCD_C_SEGMENT_LINE_6, 0x4);

    // LCD Pin36-Pin37 for changing '4' to 'A'
    LCD_C_setMemoryWithoutOverwrite(LCD_C_BASE, LCD_C_SEGMENT_LINE_36, 0x8);
    LCD_C_setMemoryWithoutOverwrite(LCD_C_BASE, LCD_C_SEGMENT_LINE_37, 0x8);

    // LCD Pin28-Pin29 for changing '5' to '6'
    LCD_C_setMemoryWithoutOverwrite(LCD_C_BASE, LCD_C_SEGMENT_LINE_28, 0x8);

    // LCD Pin14-Pin15 for changing '6' to '8'
    LCD_C_setMemoryWithoutOverwrite(LCD_C_BASE, LCD_C_SEGMENT_LINE_15, 0x4);

    //Turn LCD on
    LCD_C_on(LCD_C_BASE);

   __bis_SR_register(LPM3_bits | GIE);

	return 0;
}

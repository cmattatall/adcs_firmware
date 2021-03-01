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
//! LCD_E, 4-Mux example, display a string "123456" on LCD.
//! This examples shows how to config LCD_E in 4-Mux mode with DriverLib
//! functions on the MSP430FR4133 LaunchPad. It uses the Mode 3 (see User's Guide
//! for details) for voltage generation. Therefore the charge pump and bias 
//! voltage generator are used to generate V1, V2 and V4. V5 is connected to 
//! ground. Three external 0.1 uF capacitors are required to connected to R13, 
//! R23 and R33 respectively, and one 4.7 uF capacitor needs to connect between 
//! charge pump LCDCAP0 and LCDCAP1.
//!
//! It uses LCD_E L1~L4 as COMs and maps pin to the corresponding COM (e.g.
//! L1 -> COM1), and L4~L39 (except L27~L35) as segment pins.
//! f(LCD) = 32768Hz/((7+1)*16) = 256Hz, ACLK = default REFO ~32768Hz,
//! MCLK = SMCLK = default DCODIV ~1MHz.
//!
//!              MSP-EXP430FR4133
//!                 LaunchPad
//!             -----------------
//!         /|\|                 |
//!          | |              XIN|--
//! GND      --|RST              |  ~32768Hz
//!  |         |             XOUT|--
//!  |--0.1uF--|R13              |
//!  |--0.1uF--|R23      (L3)COM3|----------------|
//!  |--0.1uF--|R33      (L2)COM2|---------------||
//!          --|LCDC2    (L1)COM1|--------------|||
//!     4.7uF  |         (L0)COM0|-------------||||
//!          --|LCDC1            |    -------------
//!            |           L4~L39|---| 1 2 3 4 5 6 |
//!            |   except L27~L35|    -------------
//!            |                 |       TI LCD
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - LCD_E
//! - GPIO Port peripheral (for LCD_E pins)
//!
//!
//*****************************************************************************
#include "driverlib.h"

void main(void)
{
	// Hold watchdog
    WDT_A_hold(WDT_A_BASE);

    //Port select XT1
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P4,
        GPIO_PIN1 + GPIO_PIN2,
        GPIO_PRIMARY_MODULE_FUNCTION
        );

    //Set external frequency for XT1
    CS_setExternalClockSource(32768);

    //Select XT1 as the clock source for ACLK with no frequency divider
    CS_initClockSignal(CS_ACLK, CS_XT1CLK_SELECT, CS_CLOCK_DIVIDER_1);

    //Start XT1 with no time out
    CS_turnOnXT1(CS_XT1_DRIVE_0);

	//clear all OSC fault flag
	CS_clearAllOscFlagsWithTimeout(1000);

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    // L0~L26 & L36~L39 pins selected
    LCD_E_setPinAsLCDFunctionEx(LCD_E_BASE, LCD_E_SEGMENT_LINE_0, LCD_E_SEGMENT_LINE_26);
    LCD_E_setPinAsLCDFunctionEx(LCD_E_BASE, LCD_E_SEGMENT_LINE_36, LCD_E_SEGMENT_LINE_39);

    LCD_E_initParam initParams = LCD_E_INIT_PARAM;
    initParams.clockDivider = LCD_E_CLOCKDIVIDER_8;
    initParams.muxRate = LCD_E_4_MUX;
    initParams.segments = LCD_E_SEGMENTS_ENABLED;

    // Init LCD as 4-mux mode
    LCD_E_init(LCD_E_BASE, &initParams);

    // LCD Operation - Mode 3, internal 3.08v, charge pump 256Hz
    LCD_E_setVLCDSource(LCD_E_BASE, LCD_E_INTERNAL_REFERENCE_VOLTAGE, LCD_E_EXTERNAL_SUPPLY_VOLTAGE);
    LCD_E_setVLCDVoltage(LCD_E_BASE, LCD_E_REFERENCE_VOLTAGE_3_08V);

    LCD_E_enableChargePump(LCD_E_BASE);
    LCD_E_setChargePumpFreq(LCD_E_BASE, LCD_E_CHARGEPUMP_FREQ_16);

    // Clear LCD memory
    LCD_E_clearAllMemory(LCD_E_BASE);

    // Configure COMs and SEGs
    // L0, L1, L2, L3: COM pins
    // L0 = COM0, L1 = COM1, L2 = COM2, L3 = COM3
    LCD_E_setPinAsCOM(LCD_E_BASE, LCD_E_SEGMENT_LINE_0, LCD_E_MEMORY_COM0);
    LCD_E_setPinAsCOM(LCD_E_BASE, LCD_E_SEGMENT_LINE_1, LCD_E_MEMORY_COM1);
    LCD_E_setPinAsCOM(LCD_E_BASE, LCD_E_SEGMENT_LINE_2, LCD_E_MEMORY_COM2);
    LCD_E_setPinAsCOM(LCD_E_BASE, LCD_E_SEGMENT_LINE_3, LCD_E_MEMORY_COM3);

    // Display "123456"
    // LCD Pin8-Pin9 for '1'
    LCD_E_setMemory(LCD_E_BASE, LCD_E_MEMORY_BLINKINGMEMORY_4, 0x60);

    // LCD Pin12-Pin13 for '2'
    LCD_E_setMemory(LCD_E_BASE, LCD_E_MEMORY_BLINKINGMEMORY_6, 0xDB);

    // LCD Pin16-Pin17 for '3'
    LCD_E_setMemory(LCD_E_BASE, LCD_E_MEMORY_BLINKINGMEMORY_8, 0xF3);

    // LCD Pin20-Pin21 for '4'
    LCD_E_setMemory(LCD_E_BASE, LCD_E_MEMORY_BLINKINGMEMORY_10, 0x67);

    // LCD Pin4-Pin5 for '5'
    LCD_E_setMemory(LCD_E_BASE, LCD_E_MEMORY_BLINKINGMEMORY_2, 0xB7);

    // LCD Pin36-Pin37 for '6'
    LCD_E_setMemory(LCD_E_BASE, LCD_E_MEMORY_BLINKINGMEMORY_18, 0xBF);

    // Turn on LCD
    LCD_E_on(LCD_E_BASE);

    // Enter LPM3.5
    PMM_turnOffRegulator();
    __bis_SR_register(LPM4_bits | GIE);
}


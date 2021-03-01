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
//! LCD_B, 4-Mux example, display a string "430" on FH-1138P LCD.
//! This examples shows how to config LCD_B in 4-Mux mode with DriverLib
//! functions on the MSP430FG6626 target board (connected with FH-1138P LCD). The
//! internal voltage is sourced to v2 through v4 and v5 is connected to ground. 
//! Charge pump is enabled and one 4.7uF capacitor is required to connected from
//! pin LCDCAP to ground. 
//!
//! It uses LCD pin S0~S11 as segment pins.
//! f(LCD) = 32768Hz/((1+1)*2^4) = 1024Hz, ACLK = ~32768Hz,
//! MCLK = SMCLK = default DCODIV ~1MHz.
//!
//!				  MSP430FG6626
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
//!            |           S0~S11|---| 1 2 3 4 5 6 |
//!            |                 |    -------------
//!            |                 |     FH-1138P LCD
//!
//!
//! This example uses the following peripherals and I/O signals on 
//! MSP430FG6626 target board. You must review these and change as needed 
//! for your own board:
//! - LCD_B
//! - GPIO Port peripheral (for LCD pins)
//!
//!
//*****************************************************************************
#include "driverlib.h"

int main(void) {

	WDT_A_hold(WDT_A_BASE);

    //Set P5SEL P5.3/P5.4/P5.5 for LCD_B COM1/COM2/COM3
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5,
    		GPIO_PIN3|GPIO_PIN4|GPIO_PIN5);

    //Unlock Battery Backup subsystem for XT1 operation
    while(BattBak_unlockBackupSubSystem(BAK_BATT_BASE) == BATTBAK_UNLOCKFAILURE);

    //Initializes the XT1 crystal oscillator with no timeout
    //In case of failure, code hangs here.
    //For time-out instead of code hang use UCS_turnOnLFXT1WithTimeout()
    UCS_turnOnLFXT1(
        UCS_XT1_DRIVE_0,
        UCS_XCAP_3
        );

    //Set LCD functions pins from S0 to S11
	LCD_B_setPinAsLCDFunctionEx(LCD_B_BASE, LCD_B_SEGMENT_LINE_0, LCD_B_SEGMENT_LINE_11);

	//Initialize LCD
	LCD_B_initParam initParams = {0};
	initParams.clockSource = LCD_B_CLOCKSOURCE_ACLK;
	initParams.clockDivider = LCD_B_CLOCKDIVIDER_1;
	initParams.clockPrescalar = LCD_B_CLOCKPRESCALAR_16;
	initParams.muxRate = LCD_B_4_MUX;
	initParams.segments = LCD_B_SEGMENTS_ENABLED;
	LCD_B_init(LCD_B_BASE, &initParams);

	//LCD Operation - VLCD generated internally, V2-V4 generated internally, v5 to ground
	LCD_B_setVLCDSource(LCD_B_BASE, LCD_B_VLCD_GENERATED_INTERNALLY,
		LCD_B_V2V3V4_GENERATED_INTERNALLY_NOT_SWITCHED_TO_PINS,
		LCD_B_V5_VSS);

	//Set VLCD voltage to 2.60v
	LCD_B_setVLCDVoltage(LCD_B_BASE, LCD_B_CHARGEPUMP_VOLTAGE_2_60V_OR_2_17VREF);
	LCD_B_selectChargePumpReference(LCD_B_BASE, LCD_B_INTERNAL_REFERENCE_VOLTAGE);

	//Enable charge pump and select internal reference for it
	LCD_B_enableChargePump(LCD_B_BASE);

	//Disable LCD charge pump during ADC conversion
	LCD_B_configureChargePump(LCD_B_BASE, LCD_B_CHARGEPUMP_AUTO_DISABLED_DURING_ADC_CONVERSION);

	//Clear LCD memory
	LCD_B_clearAllMemory(LCD_B_BASE);

	//Display "430"
	LCD_B_setMemory(LCD_B_BASE, LCD_B_MEMORY_BLINKINGMEMORY_0, 0x67);
	LCD_B_setMemory(LCD_B_BASE, LCD_B_MEMORY_BLINKINGMEMORY_2, 0xF3);
	LCD_B_setMemory(LCD_B_BASE, LCD_B_MEMORY_BLINKINGMEMORY_4, 0xFC);

	//Turn LCD on
	LCD_B_on(LCD_B_BASE);

   __bis_SR_register(LPM3_bits | GIE);

	return 0;
}

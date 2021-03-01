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
//!
//! DMA - Repeated Single Transfer using UART 9600, TIMER_A, and ACLK
//!
//!  DMA channel 0 is used to transfer a string byte-by-byte as a repeating
//!  block to UCA1TXBUF. TIMER_A_A operates continuously at 32768Hz with CCR2IFG
//!  triggering DMA channel 0. "Hello World" is TX'd via 9600 baud UART1.
//!  ACLK= TACLK 32768Hz, MCLK= SMCLK= default DCO ~ 1048576Hz
//!  Baud rate divider with 32768hz XTAL @9600 = 32768Hz/9600 = 3.41 (000Dh 4Ah)
//!  //* An external watch crystal on XIN XOUT is required for ACLK *//
//!
//!              MSP430x552x
//!            -----------------
//!        /|\|              XIN|-
//!         | |                 | 32kHz
//!         --|RST          XOUT|-
//!           |                 |
//!           |     P4.4/UCA0TXD|-----------> "Hello World"
//!           |                 | 9600 - 8N1
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - DMA peripheral
//! - UART peripheral
//! - GPIO Port peripheral
//! - UCA0TXD
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - None.
//!
//******************************************************************************

#include "driverlib.h"

static char String1[] = { "Hello World\r\n" };

void main (void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);

    //P3.4 = USCI_A0 TXD
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P3,
        GPIO_PIN3
        );

	USCI_A_UART_initParam param1 = {0};
    param1.selectClockSource = USCI_A_UART_CLOCKSOURCE_SMCLK;
    param1.clockPrescalar = 6;
    param1.firstModReg = 13;
    param1.secondModReg = 0;
    param1.parity = USCI_A_UART_NO_PARITY;
    param1.msborLsbFirst = USCI_A_UART_LSB_FIRST;
    param1.numberofStopBits = USCI_A_UART_ONE_STOP_BIT;
    param1.uartMode = USCI_A_UART_MODE;
    param1.overSampling = USCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;

    if (STATUS_FAIL == USCI_A_UART_init(USCI_A0_BASE, &param1)){
        return;
    }
	
	//Enable UART module for operation
    USCI_A_UART_enable(USCI_A0_BASE);


    //Initialize and Setup DMA Channel 0
    /*
     * Configure DMA channel 0
     * Configure channel for repeated single transfers
     * DMA interrupt flag will be set after every 12 transfers
     * Use DMA Trigger Source 1 (TA0CCR0IFG)
     * Transfer Byte-to-byte
     * Trigger Upon Rising Edge of Trigger Source Signal
     */
    DMA_initParam param2 = {0};
    param2.channelSelect = DMA_CHANNEL_0;
    param2.transferModeSelect = DMA_TRANSFER_REPEATED_SINGLE;
    param2.transferSize = (sizeof String1 - 1);
    param2.triggerSourceSelect = DMA_TRIGGERSOURCE_1;
    param2.transferUnitSelect = DMA_SIZE_SRCBYTE_DSTBYTE;
    param2.triggerTypeSelect = DMA_TRIGGER_RISINGEDGE;
    DMA_init(&param2);
    /*
     * Configure DMA channel 0
     * Use String1 as source
     * Increment source address after every transfer
     */
    DMA_setSrcAddress(DMA_CHANNEL_0,
        (uint32_t)(uintptr_t)String1,
        DMA_DIRECTION_INCREMENT);
    /*
     * Base Address of the DMA Module
     * Configure DMA channel 0
     * Use UART TX Buffer as destination
     * Don't move the destination address after every transfer
     */
    DMA_setDstAddress(DMA_CHANNEL_0,
        USCI_A_UART_getTransmitBufferAddressForDMA(USCI_A0_BASE),
        DMA_DIRECTION_UNCHANGED);

    //Enable transfers on DMA channel 0
    DMA_enableTransfers(DMA_CHANNEL_0);

    //For DMA0 trigger
   	Timer_A_initCompareModeParam initCompareModeParam = {0};
    initCompareModeParam.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;
    initCompareModeParam.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initCompareModeParam.compareOutputMode = TIMER_A_OUTPUTMODE_OUTBITVALUE;
    initCompareModeParam.compareValue = 1;
    Timer_A_initCompareMode(TIMER_A0_BASE, &initCompareModeParam);

    //Timer sourced by SMCLK, starts in up-mode
    Timer_A_initUpModeParam initUpModeParam = {0};
    initUpModeParam.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    initUpModeParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    initUpModeParam.timerPeriod = 0x8192;
    initUpModeParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    initUpModeParam.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initUpModeParam.timerClear = TIMER_A_SKIP_CLEAR;
    initUpModeParam.startTimer = true;
    Timer_A_initUpMode(TIMER_A0_BASE, &initUpModeParam);
	

    //Enter LPM3
    _BIS_SR(LPM3_bits);
}


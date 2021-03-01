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
/*******************************************************************************
 * MSP430i20xx Flash - Flash In-System Programming, Information Segment
 *
 * Description: Unlock the Information Segment, overwrite a number of values
 * within it, then re-lock the Information Segment when writes are complete.
 * Set breakpoint on NOP in main loop to avoid stressing flash. DO NOT ERASE
 * ENTIRE INFORMATION SEGMENT, important values (TLV, etc) are contained
 * within this segment.
 *
 *                MSP430i20xx
 *             -----------------
 *         /|\|                |
 *          | |                |
 *          --|RST             |
 *            |                |
 *            |                |
 *            |                |
 *
 * Author: Yang Lu
 ******************************************************************************/
#include "driverlib.h"

// Address of the beginning of the Flash Information Segment
#define SEGSTART 0x1060
// Number of bytes within segment to write
#define SEG_LEN 16

// Value to write to segment
const uint32_t Value = 0xBEEFDEAD;

void write_InfoSeg(uint32_t value);

void main(void) {

    // Pointer to beginning of Flash segment
    uint32_t *flashPtr = (uint32_t *)SEGSTART;

    // Stop WDT
    WDT_hold(WDT_BASE);

    // Setting the DCO to use the internal resistor. DCO will be at 16.384MHz
    CS_setupDCO(CS_INTERNAL_RESISTOR);

    // Setting MCLK to DCO / 1. MCLK = 16.384MHz.
    CS_initClockSignal(CS_MCLK, CS_CLOCK_DIVIDER_1);

    // MCLK for Flash Timing Generator
    // Flash clock will run at ~390kHz. Datasheet recommends 257kHz - 476kHz
    FlashCtl_setupClock(390095, 16384000, FLASHCTL_MCLK);

    FlashCtl_unlockInfo();
    FlashCtl_fillMemory32(Value, flashPtr, SEG_LEN / 4);
    FlashCtl_lockInfo();

    while(1) {
        // Set breakpoint to view memory
        __no_operation();
    }
}

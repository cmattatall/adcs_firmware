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

//******************************************************************************
//!
//! DMA - Repeated Block Transfer to-and-from RAM, Software Trigger.
//!
//!  A 16 word block from 1C00-1C1Fh is transfered to 1C20h-1C3fh using DMA0 in
//!  a burst block using software DMAREQ trigger. After each transfer, source,
//!  destination and DMA size are reset to initial software setting because DMA
//!  transfer mode 5 is used. P1.0 is toggled during DMA transfer only for
//!  demonstration purposes.
//!  ** RAM location 0x1C00 - 0x1C3F used - make sure no compiler conflict **
//!  ACLK = REFO = 32kHz, MCLK = SMCLK = default DCO 1048576Hz
//!
//!   Tested on MSP430F5529, MSP430FR5739
//!             -----------------
//!         /|\|              XIN|-
//!          | |                 | 32kHz
//!          --|RST          XOUT|-
//!            |                 |
//!            |             P1.0|-> LED
//!            |                 |
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - DMA peripheral
//! - GPIO Port peripheral
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - None.
//!
//******************************************************************************

void main (void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);

    //Set P1.0 to output direction
    GPIO_setAsOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN0
        );

    //Initialize and Setup DMA Channel 0
    /*
     * Base Address of the DMA Module
     * Configure DMA channel 0
     * Configure channel for repeated block transfers
     * DMA interrupt flag will be set after every 16 transfers
     * Use DMA_startTransfer() function to trigger transfers
     * Transfer Word-to-Word
     * Trigger upon Rising Edge of Trigger Source Signal
     */
    DMA_initParam param = {0};
    param.channelSelect = DMA_CHANNEL_0;
    param.transferModeSelect = DMA_TRANSFER_REPEATED_BLOCK;
    param.transferSize = 16;
    param.triggerSourceSelect = DMA_TRIGGERSOURCE_0;
    param.transferUnitSelect = DMA_SIZE_SRCWORD_DSTWORD;
    param.triggerTypeSelect = DMA_TRIGGER_RISINGEDGE;
    DMA_init(&param);
    /*
     * Base Address of the DMA Module
     * Configure DMA channel 0
     * Use 0x1C00 as source
     * Increment source address after every transfer
     */
    DMA_setSrcAddress(
        DMA_CHANNEL_0,
        0x1C00,
        DMA_DIRECTION_INCREMENT);
    /*
     * Base Address of the DMA Module
     * Configure DMA channel 0
     * Use 0x1C20 as destination
     * Increment destination address after every transfer
     */
    DMA_setDstAddress(
        DMA_CHANNEL_0,
        0x1C20,
        DMA_DIRECTION_INCREMENT);

    //Enable transfers on DMA channel 0
    DMA_enableTransfers(
        DMA_CHANNEL_0);

    while (1)
    {
        //set P1.0
        GPIO_setOutputHighOnPin(
            GPIO_PORT_P1,
            GPIO_PIN0
            );

        //Start block tranfer on DMA channel 0
        DMA_startTransfer(
            DMA_CHANNEL_0);

        //Clear P1.0 LED off
        GPIO_setOutputLowOnPin(
            GPIO_PORT_P1,
            GPIO_PIN0
            );
    }
}


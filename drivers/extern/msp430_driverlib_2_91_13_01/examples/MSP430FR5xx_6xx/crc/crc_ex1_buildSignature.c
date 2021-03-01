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
//! CRC - Build Signature and Rebuild to test.
//!
//!  The CRC is first used to build a signature using a seed and multiple data
//!  values. This signature is considered as the checksum and can be sent by a
//!  UART connection along with the data to verify the correct data has been
//!  sent. The second half of this program is used to test the CRC checksum of
//!  the data that has been created, by recreating the same checksum and
//!  comparing it to the first checksum. If the two checksum are equal, then
//!  P1.0 is set and the LED is turned on. Breakpoints can be set before the
//!  CRC_setData() function to observe the CRC register values and see the
//!  subsequent signatures before/after each data set.
//!
//!  Tested On:   MSP430FR5969
//!             -----------------
//!         /|\|                 |
//!          | |                 |
//!          --|RST              |
//!            |             P1.0|--> LED
//!            |                 |
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - CRC peripheral
//! - GPIO Port peripheral
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - NONE
//!
//******************************************************************************
#include "driverlib.h"

void main (void)
{
    uint16_t crcSeed = 0xBEEF;
    uint16_t data[] = {0x0123,
                           0x4567,
                           0x8910,
                           0x1112,
                           0x1314};
    uint16_t crcResult;
    uint8_t i;

    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    //Set P1.0 as an output
    GPIO_setAsOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN0);

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    //Set the CRC seed
    CRC_setSeed(CRC_BASE,
        crcSeed);

    for (i = 0; i < 5; i++)
    {
        //Add all of the values into the CRC signature
        CRC_set16BitData(CRC_BASE,
            data[i]);
    }

    //Save the current CRC signature checksum to be compared for later
    crcResult = CRC_getResult(CRC_BASE);


    //*** Test the CRC ***//

    //Reset the CRC Signature and set the same seed
    CRC_setSeed(CRC_BASE,
        crcSeed);

    for (i = 0; i < 5; i++)
    {
        //Add all of the values into the CRC signature
        CRC_set16BitData(CRC_BASE,
            data[i]);
    }

    //Compare the 2 CRC Resulting Signature Checksums
    if (crcResult == CRC_getResult(CRC_BASE)){
        //set P1.0
        GPIO_setOutputHighOnPin(
            GPIO_PORT_P1,
            GPIO_PIN0);
    }

    //Enter LPM4, interrupts enabled
    __bis_SR_register(LPM4_bits);
    __no_operation();
}


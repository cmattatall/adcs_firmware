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
//! CRC - Calculate CRC and check result
//!
//!	 Breakpoints can be set at __no_operation()to compare hardware and software
//!  generated CRC values.
//!
//!  Tested On:   MSP430FR6989
//!             -----------------
//!         /|\|                 |
//!          | |                 |
//!          --|RST              |
//!            |             	 |
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

#define CRC16_POLY              0x1021
#define CRC16_INIT				0xFFFF

static const uint8_t myData[9] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};
static uint16_t calculateCRC16(uint8_t* data, uint32_t length);

int main(void)
{
    volatile uint16_t hwCalculatedCRC, swCalculatedCRC;
    uint32_t ii;

   //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    CRC32_setSeed(CRC16_INIT, CRC16_MODE);

    for(ii=0;ii<9;ii++)
    	CRC32_set8BitDataReversed(myData[ii], CRC16_MODE);

    /* Getting the result from the hardware module */
    hwCalculatedCRC = CRC32_getResult(CRC16_MODE);

    /* Calculating the CRC16 checksum through software */
    swCalculatedCRC = calculateCRC16((uint8_t*)myData, 9);

    /* Pause for the debugger */
    __no_operation();
}


/* Standard software calculation of CRC16 */
static uint16_t calculateCRC16(uint8_t* data, uint32_t length)
{
    uint16_t i, j;
    uint16_t msg;
    uint16_t crc = 0xFFFF;
    uint8_t* pmsg = data;

    uint32_t msg_size = length;

    for(i = 0 ; i < msg_size ; i ++)
    {
        msg = (pmsg[i] << 8);
        for(j = 0 ; j < 8 ; j++)
        {
            if((msg ^ crc) >> 15) crc = (crc << 1) ^ CRC16_POLY;
            else crc <<= 1;
            msg <<= 1;
        }
    }

    return(crc);
}



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
//!   Dummy calibration data is written to infoA.
//!   16 locations are written with char data, 8 locations with int data and
//!   4 locations with word data.  LED toggles upon completion.
//!
//!   Check infoB location @ 0x1980 on execution of program.
//!
//!   Tested On: MSP430F5529
//!             -----------------
//!         /|\|                 |
//!          | |             P1.0|-LED
//!          --|RST              |
//!            |                 |
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - Flash module
//! - GPIO Port peripheral
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - None.
//********************************************************************************
#include "driverlib.h"

//*****************************************************************************
//
//INFOB address in flash into which calibration data would be written
//
//*****************************************************************************
#define INFOA_START   (0x1980)

//*****************************************************************************
//
//Dummy char array of 'calibration data' - to be stored in flash
//
//*****************************************************************************
uint8_t calibration_data_char[16] = {0x00,
                                           0x01,
                                           0x02,
                                           0x03,
                                           0x04,
                                           0x05,
                                           0x06,
                                           0x07,
                                           0x08,
                                           0x09,
                                           0x10,
                                           0x11,
                                           0x12,
                                           0x13,
                                           0x14,
                                           0x15};
//*****************************************************************************
//
//Dummy int array of 'calibration data' - to be stored in flash
//
//*****************************************************************************
uint16_t calibration_data_int[8] = {0x1617,
                                        0x1819,
                                        0x2021,
                                        0x2223,
                                        0x2425,
                                        0x2627,
                                        0x2829,
                                        0x3031};
//*****************************************************************************
//
//Dummy long array of 'calibration data' - to be stored in flash
//
//*****************************************************************************
uint32_t calibration_data_long[4] = {0x35343332,
                                          0x39383736,
                                          0x43424140,
                                          0x47464544};

void main (void)
{
    uint16_t status;

    WDT_A_hold(WDT_A_BASE);

    //Set P1.0 to output direction
    GPIO_setAsOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN0
        );

    //Unlock Info Segment A
    FlashCtl_unlockInfoA();

    do
    {
        //Erase INFOA
        FlashCtl_eraseSegment((uint8_t *)INFOA_START);
        status = FlashCtl_performEraseCheck((uint8_t *)INFOA_START,
            128
            );
    } while (status == STATUS_FAIL);

    //Write calibration data to INFOA
    FlashCtl_write8(calibration_data_char,
        (uint8_t *)INFOA_START,
        16
        );
    FlashCtl_write16(calibration_data_int,
        (uint16_t *)(INFOA_START + 16),
        8
        );
    FlashCtl_write32(calibration_data_long,
        (uint32_t *)(INFOA_START + 32),
        4
        );
    FlashCtl_fillMemory32(0xBEEFBEEF,
        (uint32_t *)(INFOA_START + 48),
        (128 - 48) >> 2
        );

    //Lock Info Segment A
    FlashCtl_lockInfoA();

    while (1)
    {
        //Toggle P1.0
        GPIO_toggleOutputOnPin(
            GPIO_PORT_P1,
            GPIO_PIN0
            );

        //Delay
        __delay_cycles(1000000);
    }
}

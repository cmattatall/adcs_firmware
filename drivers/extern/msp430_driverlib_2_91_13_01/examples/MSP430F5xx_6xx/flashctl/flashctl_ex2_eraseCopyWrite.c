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
//!  This program first erases flash seg C, then it increments all
//!  values in seg C, then it erases seg D, then copies seg C to seg D. LED
//!  toggles upon completion.  Starting addresses of segments defined
//!  in this code: Seg C-0x1880, Seg D-0x1800.
//!  ACLK = REFO = 32kHz, MCLK = SMCLK = default DCO 1048576Hz
//!
//!  Tested On: MSP430F5529
//!            -----------------
//!        /|\|                 |
//!         | |             P1.0|-LED
//!         --|RST              |
//!           |                 |
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
//*****************************************************************************
#include "driverlib.h"
//*****************************************************************************
//
//INFO address in flash from which data is copied
//
//*****************************************************************************
#define INFOC_START   (0x1880)
//*****************************************************************************
//
//INFO address in flash to which data is copied into
//
//*****************************************************************************
#define INFOD_START   (0x1800)

//*****************************************************************************
//
//Number of bytes being copied
//
//*****************************************************************************
#define NUMBER_OF_BYTES   128

//*****************************************************************************
//
//Dummy array of 'calibration data' - to be stored in flash
//
//*****************************************************************************
uint8_t calibration_data[NUMBER_OF_BYTES];

//*****************************************************************************
//
//Delay cycles
//
//*****************************************************************************
#define DELAY_TIME 1000000

//*****************************************************************************
//
//Function prototypes
//
//*****************************************************************************
void write_SegC (void);
void copy_C2D (void);

void main (void)
{
    volatile uint16_t i;
    uint8_t value = 0;

    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    //Set P1.0 to output direction
    GPIO_setAsOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN0
        );

    //fill value for calibration data
    for (i = 0; i < NUMBER_OF_BYTES; i++)
    {
        calibration_data[i] = value++;
    }

    //Write segment C, increment value
    write_SegC();

    //Copy segment C to D
    copy_C2D();

    //continuous loop
    while (1)
    {
        //Toggle P1.0
        GPIO_toggleOutputOnPin(
            GPIO_PORT_P1,
            GPIO_PIN0
            );

        //Delay
        __delay_cycles(DELAY_TIME);
    }
}

//------------------------------------------------------------------------------
//Input = value, holds value to write to Seg C
//------------------------------------------------------------------------------
void write_SegC ()
{
    uint16_t status;

    //Erase INFOC
    do
    {
        FlashCtl_eraseSegment((uint8_t *)INFOC_START);
        status = FlashCtl_performEraseCheck((uint8_t *)INFOC_START,
            NUMBER_OF_BYTES
            );
    } while (status == STATUS_FAIL);

    //Flash Write
    FlashCtl_write8(calibration_data,
        (uint8_t *)INFOC_START,
        NUMBER_OF_BYTES
        );
}

//------------------------------------------------------------------------------
//Copy Seg C to Seg D
//------------------------------------------------------------------------------
void copy_C2D (void)
{
    char *FLASH_ptrC;
    char *FLASH_ptrD;

    uint16_t status;

    //Erase INFOD
    do {
        FlashCtl_eraseSegment((uint8_t *)INFOD_START);
        status = FlashCtl_performEraseCheck((uint8_t *)INFOD_START,
            NUMBER_OF_BYTES
            );
    } while (status == STATUS_FAIL);

    //Initialize Flash segment C ptr
    FLASH_ptrC = (char *)INFOC_START;

    //Initialize Flash segment D ptr
    FLASH_ptrD = (char *)INFOD_START;

    //Flash Write
    FlashCtl_write8((uint8_t *)FLASH_ptrC,
        (uint8_t *)FLASH_ptrD,
        NUMBER_OF_BYTES
        );
}


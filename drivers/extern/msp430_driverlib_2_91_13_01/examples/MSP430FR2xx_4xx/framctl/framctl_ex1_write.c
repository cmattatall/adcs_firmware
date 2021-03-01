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
//!   Long word writes to FRAM
//!
//!   Description: Use long word write to write to 512 byte blocks of FRAM.
//!   Toggle LEDs after every 100 writes.
//!   NOTE: Running this example for extended periods will impact the FRAM
//!   endurance.
//!   ACLK = VLO, MCLK = SMCLK = ~1.048MHz
//!
//!           MSP430FR2xx_4xx Board
//!              ---------------
//!          /|\|               |
//!           | |               |
//!           --|RST            |
//!             |               |
//!             |               |
//!             |               |-->LED1
//!             |               |-->LED2
//!
//******************************************************************************
#include "driverlib.h"
#include "Board.h"

#define FRAM_TEST_START 0xCABA

void main(void) {
    uint8_t count = 0;
    uint32_t data = 0;

    // Stop WDT
    WDT_A_hold(WDT_A_BASE);

    //Set LED1 as an output pin.
    GPIO_setAsOutputPin(
            GPIO_PORT_LED1,
            GPIO_PIN_LED1);

    //Set LED2 as an output pin.
    GPIO_setAsOutputPin(
            GPIO_PORT_LED2,
            GPIO_PIN_LED2);

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    // Initialize dummy data
    data = 0x11111111;

    while (1) {
        data += 0x00010001;

        FRAMCtl_fillMemory32(data, (uint32_t *) FRAM_TEST_START,
                128);
        count++;
        if (count > 100) {
            //Toggle LED1 output pin.
            GPIO_toggleOutputOnPin(GPIO_PORT_LED1, GPIO_PIN_LED1);
            //Toggle LED2 output pin.
            GPIO_toggleOutputOnPin(GPIO_PORT_LED2, GPIO_PIN_LED2);

            //Reset counter
            count = 0;
            data = 0x11111111;
        }
    }
}

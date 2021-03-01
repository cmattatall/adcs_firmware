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
// This example shows transitions between a user Reset via the reset button, a
// software triggered BOR, and entrance into LPM3.5.
// Reading the reset interrupt flag from PMMIFG register is equivalent
// to reading from reset vector register.
//*******************************************************************************
//
//  1) Power on device:
//     LED1 and LED2 off: device is running and was not in LPM3.5 mode previously.
//
//  2) Press User Reset button:
//
//     LED1 turns on, device is running and has reset due to RST pin
//     LED2 toggles on/off: device has reset due to a software BOR
//     LED1 and LED2 on: device is now in LPM3.5
//
//!       MSP430FR2xx_4xx Board
//!         -----------------
//!     /|\|                 |
//!      | |                 |
//!      | |                 |
//!      --|RST              |
//!        |                 |-->LED1
//
//********************************************************************************
#include "driverlib.h"
#include "Board.h"

void main(void)
{
    //Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    /*
    * By default, the pins are unlocked unless waking
    * up from an LPMx.5 state in which case all GPIO
    * are previously locked.
    *
    */
    PMM_unlockLPM5();
    //Set LED1 as output pins.
    GPIO_setAsOutputPin(
        GPIO_PORT_LED1,
        GPIO_PIN_LED1
    );
    //Set LED2 as output pins.
    GPIO_setAsOutputPin(
        GPIO_PORT_LED2,
        GPIO_PIN_LED2
    );

    //Was this reset triggered by the Reset flag?
    if (PMM_getInterruptStatus(PMM_RST_INTERRUPT))
    {
        //Clear reset flag
        PMM_clearInterrupt(PMM_RST_INTERRUPT);
        GPIO_setOutputHighOnPin(
            GPIO_PORT_LED1,
            GPIO_PIN_LED1
        );
        //Create a visual delay
        __delay_cycles(1000000);

        //Trigger a software Brown Out Reset (BOR)
        /*
        * Base Address of PMM,
        * Forces the devices to perform a BOR.
        */
        //Software trigger a BOR.
        PMM_trigBOR();
    }

    //Was this reset triggered by the BOR flag?
    if (PMM_getInterruptStatus(PMM_BOR_INTERRUPT))
    {
        //Clear BOR flag
        PMM_clearInterrupt(PMM_BOR_INTERRUPT);
        GPIO_toggleOutputOnPin(
            GPIO_PORT_LED2,
            GPIO_PIN_LED2
        );
        //Create a visual delay
        __delay_cycles(1000000);
        GPIO_toggleOutputOnPin(
            GPIO_PORT_LED2,
            GPIO_PIN_LED2
        );

        //Disable Regulator
        /*
         * Base Address of PMM,
         * Regulator is turned off when going to LPM3/4.
         * System enters LPM3.5 or LPM4.5, respectively.
         */
        PMM_turnOffRegulator();
        // Enter LPM3.5
        __bis_SR_register(LPM3_bits);
    }

    while (1)
    {
        // Don't sleep
        __no_operation();
    }
}

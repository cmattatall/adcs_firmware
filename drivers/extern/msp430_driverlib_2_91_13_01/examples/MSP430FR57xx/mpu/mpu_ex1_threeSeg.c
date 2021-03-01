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
//!  MSP430FR57x Demo - MPU Write protection violation - PUC
//!
//!  Description: The MPU segment boundaries are defined by:
//!  Border 1 = 0xCA00 [segment #: 5]
//!  Border 2 = 0xD000 [segment #: 8]
//!  Segment 1 = 0xC200 - 0xC9FF
//!  Segment 2 = 0xCA00 - 0xCFFF
//!  Segment 3 = 0xD000 - 0xFFFF
//!  Segment 2 is write protected. Any write to an address in the segment 2 range
//!  causes a PUC. The LEDs toggles due to repeated PUCs. A delay is included
//!  so the debugger can gain access via JTAG.
//!
//!  ACLK = n/a, MCLK = SMCLK = TACLK = default DCO = ~625 KHz
//!
//!           MSP430FR5739
//!         ---------------
//!     /|\|               |
//!      | |               |
//!      --|RST            |
//!        |               |
//!		   |           P1.0|-->LED
//!        |           P3.5|-->LED
//!
//******************************************************************************

#include "driverlib.h"

uint16_t *ptr = 0;
uint16_t Data =0;

void main(void)
{
    WDT_A_hold(WDT_A_BASE);                 // Stop WDT

    //Set P1.0 as an output pin.
    /*

     * Select Port 1
     * Set Pin 0 as output
     */
    GPIO_setAsOutputPin(
    		GPIO_PORT_P1,
    		GPIO_PIN0);

    //Set P3.5 as an output pin.
    /*

    * Select Port 3
    * Set Pin 5 as output
    */

    GPIO_setAsOutputPin(
    		GPIO_PORT_P3,
    		GPIO_PIN5
    );

    if(MPU_getInterruptStatus(MPU_BASE,MPUSEG2IFG))
    {
        //Toggle P1.0 output pin.
        /*

        * Select Port 1
        * Set Pin 0 as output
         */

        GPIO_toggleOutputOnPin(
        	GPIO_PORT_P1,
        	GPIO_PIN0
        );

        //Toggle P3.5 output pin.
        /*

        * Select Port 3
        * Set Pin 5 as output
         */

        GPIO_toggleOutputOnPin(
        	GPIO_PORT_P3,
        	GPIO_PIN5
        );
    }

    __delay_cycles(30000);                    // Delay to see toggle

    MPU_enablePUCOnViolation(MPU_BASE,MPU_SECOND_SEG);

    MPU_initThreeSegmentsParam param = {0};
    param.seg1boundary = 0x05;
    param.seg2boundary = 0x08;
    param.seg1accmask = MPU_READ|MPU_WRITE|MPU_EXEC;
    param.seg2accmask = MPU_READ;
    param.seg3accmask = MPU_READ|MPU_WRITE|MPU_EXEC;
    MPU_initThreeSegments(MPU_BASE, &param);

    MPU_start(MPU_BASE);

    Data = 0x88;

    // Cause an MPU violation by writing to segment 2
    ptr = (uint16_t  *)0xCC02;
    *ptr = Data;


    while(1);                                 // Code never gets here
}



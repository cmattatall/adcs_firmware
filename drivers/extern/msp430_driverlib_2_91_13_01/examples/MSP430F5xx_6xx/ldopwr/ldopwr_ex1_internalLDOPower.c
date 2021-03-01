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
//!   MSP430F530x Demo - Internal LDO powering device VCC and PortU
//!
//!   Description: This example code shows the device can be automatically
//!   powered up by the internal LDO when sufficicnt is present at the LDOI pin
//!   and how the external connection between LDOO and VCC can be used to power
//!   the device VCC.
//!
//!   Note:
//!    1.To power the MSP430 device VCC via the internal LDO, connect  LDOO to
//!      VCC externally. If using TI target board (MSP-TS430RGC64B), jumper pin
//!      header JP4
//!    2.Ensure current drawn by device VCC is < ILDOO spec
//!
//!   ACLK = 32.768kHz, MCLK = SMCLK = default DCO~1MHz
//!
//!                MSP430F530x
//!             -----------------
//!        /|\|              VCC|<-
//!         | |                 |  |(jumper JP4 on MSP-TS430RGC64B)
//!         --|RST          LDOO|--
//!           |                 |
//!           |             LDOI|<-- LDO Input Voltage (3.76V to 5.5V -
//!           |                 |   refer to the d/s specs for actual range)
//!           |             PU.0|--> Toggle
//!           |             PU.1|--> Toggle (complementary to PU.0)
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - LDO-PWR module
//! - GPIO Port peripheral
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - LDO_PWR_VECTOR.
//!
//******************************************************************************

#include "driverlib.h"

void main (void)
{
    volatile uint16_t i;

    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    //Enable access to config registers
    LDOPWR_unLockConfiguration(LDOPWR_BASE);

    //Configure PU.0 as output pins
    LDOPWR_enablePort_U_outputs(LDOPWR_BASE);

    //Set PU.1 = high
    LDOPWR_setPort_U1_outputData(LDOPWR_BASE,
        LDOPWR_PORTU_PIN_HIGH
        );

    //Set PU.0 = low
    LDOPWR_setPort_U0_outputData(LDOPWR_BASE,
        LDOPWR_PORTU_PIN_LOW
        );

    //Enable LDO overload indication interrupt
	LDOPWR_clearInterrupt(LDOPWR_BASE,
        LDOPWR_LDO_OVERLOAD_INDICATION_INTERRUPT
        );
    LDOPWR_enableInterrupt(LDOPWR_BASE,
        LDOPWR_LDO_OVERLOAD_INDICATION_INTERRUPT
        );

    //Disbale access to config registers
    LDOPWR_lockConfiguration(LDOPWR_BASE);

    //continuous loop
    while (1)
    {
        //Delay
        for (i = 50000; i > 0; i--){}

        //Enable access to config registers
        LDOPWR_unLockConfiguration(LDOPWR_BASE);

        //XOR PU.0/1
        LDOPWR_togglePort_U1_outputData(LDOPWR_BASE);
        LDOPWR_togglePort_U0_outputData(LDOPWR_BASE);

        //Disbale access to config registers
        LDOPWR_lockConfiguration(LDOPWR_BASE);
    }
}

//******************************************************************************
//
//This is the LDO_PWR_VECTOR interrupt vector service routine.
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=LDO_PWR_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(LDO_PWR_VECTOR)))
#endif
void LDOInterruptHandler (void)
{
    if (LDOPWR_getInterruptStatus(LDOPWR_BASE,
            LDOPWR_LDO_OVERLOAD_INDICATION_INTERRUPT
            )){
        //Enable access to config registers
        LDOPWR_unLockConfiguration(LDOPWR_BASE);

        //Software clear IFG
        LDOPWR_clearInterrupt(LDOPWR_BASE,
            LDOPWR_LDO_OVERLOAD_INDICATION_INTERRUPT
            );

        //Disable access to config registers
        LDOPWR_lockConfiguration(LDOPWR_BASE);

        //Over load indication; take necessary steps in application firmware
        while (1) ;
    }
}


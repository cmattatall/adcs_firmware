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
 * MSP430i2xx Power Management Module - Enter LPM4.5
 *
 * Description: In this example, the device is setup to enter LPM4.5. Before
 * the device enters LPM4.5 the LED is turned on to indicate it has entered
 * LPM4.5. Exiting LPM4.5 is triggered by a Low to High transition on P2.1 that
 * causes a reset of the device. Once reset the LPM4.5 interrupt is set and the
 * device begins to simply toggle the LED. This example demonstrates how to
 * setup the device for LPM4.5 and interrupt successfully from it.
 *
 *
 *                MSP430i2041
 *             ------------------
 *         /|\|                  |
 *          | |              P1.4|-->LED
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |              P2.1|-->Wakeup
 *            |                  |
 *            |                  |
 *
 * Author: Zack Lalanne
 ******************************************************************************/

#include "driverlib.h"

#define GPIO_PIN_ALL (GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |          \
                      GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7)

int main(void) {

    WDT_hold(WDT_BASE);

    // Configure GPIO for low current numbers while in LPM4.5
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN_ALL);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN_ALL);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN_ALL);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN_ALL);

    // Determine if coming out of LPM 4.5
    if(PMM_getInterruptStatus(PMM_LPM45_INTERRUPT)) {

        PMM_clearInterrupt(PMM_LPM45_INTERRUPT);

        // Need to unlock IO after exiting LPM4.5 so LED will blink
        PMM_unlockIOConfiguration();

    } else {

        // Configure exit of LPM4.5 on P2.1 interrupt
        GPIO_setAsInputPin(GPIO_PORT_P2, GPIO_PIN1);
        GPIO_selectInterruptEdge(GPIO_PORT_P2, GPIO_PIN1,
                                 GPIO_LOW_TO_HIGH_TRANSITION);
        GPIO_clearInterrupt(GPIO_PORT_P2, GPIO_PIN1);
        GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN1);

        // Turn on LED to indicate we are about to enter LPM4.5
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN4);

        // Turn off regulator so we can enter LPM4.5
        PMM_setRegulatorStatus(PMM_REGULATOR_OFF);

        __bis_SR_register(LPM4_bits);
    }

    // Slow down clock so we can see LED blink
    // Configure MCLK = ~1MHz
    CS_initClockSignal(CS_MCLK, CS_CLOCK_DIVIDER_16);

    while(1) {
        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN4);
        __delay_cycles(100000);
    }
}

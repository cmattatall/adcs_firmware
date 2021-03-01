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
 * MSP430i2xx TLV - Check Checksum
 *
 * Description: In this example, the checksum of the TLV is checked to make sure
 * the TLV has not been corrupted. The LED is turned on if there are no errors
 * with the TLV. To cause an error, backup the TLV first and then erase part of
 * it. This will cause a checksum error to occur and the LED will not turn on.
 *
 *
 *                MSP430i2041
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |              P1.4|-->LED
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *
 * Author: Zack Lalanne
 ******************************************************************************/

#include "driverlib.h"

int main(void) {

    bool result;

    // Stop the WDT
    WDT_hold(WDT_BASE);

    // Check the TLV checksum
    result = TLV_performChecksumCheck();

    // Turn on LED if test passed
    if(result) {
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN4);
    } else {
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN4);
    }

    // LED for indicating checksum result
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN4);

    __bis_SR_register(LPM0_bits);
}

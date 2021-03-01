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
 * MSP430i2xx GPIO - Input Interrupt
 *
 * Description: In this example, P1.4 is setup as an output pin. P2.0 is set up
 * as an input pin that interrupts when the voltage applied goes from high to
 * low. When the voltage transition happens the ISR is fired and P1.4 is
 * toggled to show that an interrupt happened. If P2.0 is still low then the
 * interrupt edge is switched to trigger when P2.0 goes from low to high. This
 * way an interrupt occurs on every voltage change of P2.0.
 *
 *
 *                MSP430i2041
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST           P2.0|<--Input Signal
 *            |                  |
 *            |                  |
 *            |              P1.4|-->LED
 *            |                  |
 *            |                  |
 *
 * Author: Zack Lalanne
 ******************************************************************************/

#include "driverlib.h"

int main(void) {

    WDT_hold(WDT_BASE);

    // Set P1.4 as output pin to drive the LED
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN4);

    // Setup P2.0 to interrupt on High to Low transition
    GPIO_setAsInputPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_selectInterruptEdge(GPIO_PORT_P2, GPIO_PIN0,
                             GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_clearInterrupt(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN0);

    // Go into LPM0 with interrupts enabled
    __bis_SR_register(LPM0_bits | GIE);
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(PORT2_VECTOR)))
#endif
void PORT2_ISR(void) {

    switch(__even_in_range(P2IV, 0x10)){
        case 0x00: break;               // No interrupt
        case 0x02:                      // P2.0 interrupt
            // Check current state of P2.0 and set correct interrupt edge to occur
            if(GPIO_getInputPinValue(GPIO_PORT_P2, GPIO_PIN0) == GPIO_INPUT_PIN_HIGH) {
                GPIO_selectInterruptEdge(GPIO_PORT_P2, GPIO_PIN0,
                                         GPIO_HIGH_TO_LOW_TRANSITION);
            } else {
                GPIO_selectInterruptEdge(GPIO_PORT_P2, GPIO_PIN0,
                                         GPIO_LOW_TO_HIGH_TRANSITION);
            }

            // Toggle 1.4 to indicate an interrupt occurred
            GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN4);
            break;
        case 0x04: break;               // P2.1 interrupt
        case 0x06: break;               // P2.2 interrupt
        case 0x08: break;               // P2.3 interrupt
        case 0x0A: break;               // P2.4 interrupt
        case 0x0C: break;               // P2.5 interrupt
        case 0x0E: break;               // P2.6 interrupt
        case 0x10: break;               // P2.7 interrupt
    }
}

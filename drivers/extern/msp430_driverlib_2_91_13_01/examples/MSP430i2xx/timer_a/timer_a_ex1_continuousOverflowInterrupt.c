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
 * MSP430i2xx Timer_A - Continuous Mode Overflow Interrupt
 *
 * Description: In this example, the 16 bit Timer is configured in continuous
 * mode and setup to interrupt on an overflow. The timer uses ACLK as its
 * source running at 32kHz. Therefore 2^16 counts at 32kHz will take ~2.048
 * seconds to overflow. The LED show toggle at a period of ~4.096 seconds.
 *
 *                MSP430i2041
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST           P1.4|--->LED
 *            |                  |
 *            |                  |
 *            |                  |
 *
 *
 * Author: Zack Lalanne
 ******************************************************************************/
#include "driverlib.h"

int main(void) {

    Timer_A_initContinuousModeParam continuousModeConfig = {
        TIMER_A_CLOCKSOURCE_ACLK,           // ACLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_1,      // ACLK/1 = 32kHz
        TIMER_A_TAIE_INTERRUPT_ENABLE,      // Enable Overflow ISR
        TIMER_A_DO_CLEAR                    // Clear Counter
    };

    WDT_hold(WDT_BASE);

    // Setting P1.4 as LED Pin
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN4);

    // Setting the DCO to use the internal resistor. DCO will be at 16.384MHz
    // ACLK is at 32kHz
    CS_setupDCO(CS_INTERNAL_RESISTOR);

    // Configure the timer to use ACLK and interrupt on overflow
    Timer_A_initContinuousMode(TIMER_A0_BASE, &continuousModeConfig);

    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);

    // Go into LPM0 with interrupts enabled
    __bis_SR_register(LPM0_bits | GIE);
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER0_A1_VECTOR)))
#endif
void TA0_ISR(void) {
    switch(__even_in_range(TAIV, TA0IV_TAIFG)) {
        case TA0IV_NONE: break;
        case TA0IV_TACCR1: break;
        case TA0IV_TACCR2: break;
        case TA0IV_TAIFG:
            GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN4);
            break;
        default:   break;
    }
}

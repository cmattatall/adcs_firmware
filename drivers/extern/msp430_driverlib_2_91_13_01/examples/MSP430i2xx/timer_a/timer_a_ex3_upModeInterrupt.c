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
 * MSP430i2xx Timer_A - Up Mode Interrupt
 *
 * Description: In this example, the 16 bit Timer_A is configured in up mode and
 * setup to interrupt once the timer hits the value defined by the user. The
 * timer uses SMCLK divided down to 256kHz as its clock source. This example is
 * useful if you are trying to perform a periodic task, and can easily be
 * adapted by changing the timer period and clock dividers. Once the timer is
 * equal to the value in CCR0 an interrupt occurs signaling that the desired
 * time has past. CCR0 does not share an interrupt vector so all interrupt flag
 * checking and clearing is performed by hardware. In the given example the
 * LED should toggle every 250ms. The period calculation can be seen below.
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

// 250ms Period Calculation:
// SMCLK = DCO / 8 = 2.048MHz
// TACLK = SMCLK / 8 = 256kHz
// 250ms / (1 / 256kHz) = 64000
#define TIMER_PERIOD 64000

int main(void) {

    Timer_A_initUpModeParam upModeConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_8,      // SMCLK/8 = 256kHz
        TIMER_PERIOD,                       // 250ms
        TIMER_A_TAIE_INTERRUPT_DISABLE,     // Disable Overflow ISR
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE, // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                    // Clear Counter
    };

    WDT_hold(WDT_BASE);

    // Setting P1.4 as LED Pin
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN4);

    // Setting the DCO to use the internal resistor. DCO will be at 16.384MHz
    // SMCLK = DCO / 8 = 2.048MHz
    CS_setupDCO(CS_INTERNAL_RESISTOR);
    CS_initClockSignal(CS_SMCLK, CS_CLOCK_DIVIDER_8);

    // Configure the timer to use ACLK and interrupt when timer reaches CCR0
    Timer_A_initUpMode(TIMER_A0_BASE, &upModeConfig);

    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    // Go into LPM0 with interrupts enabled
    __bis_SR_register(LPM0_bits | GIE);
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER0_A0_VECTOR)))
#endif
void TA0_ISR(void) {
    // TIMER0_A0_VECTOR only contains CCR0 interrupt
    // No need to check/clear interrupt flags, toggle LED to show ISR reached
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN4);
}

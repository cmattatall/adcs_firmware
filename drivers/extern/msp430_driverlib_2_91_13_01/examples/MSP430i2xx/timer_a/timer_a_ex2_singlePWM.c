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
 * MSP430i2xx Timer_A - Single PWM
 *
 * Description: In this example, the 16 bit Timer_A is configured to output a
 * 75% duty cycle PWM on P1.5. The timer uses SMCLK / 4 for a clock source and
 * outputs a PWM signal with a 8ms period. The PWM generation does not require
 * any interrupts and stays in LPM0. Calculations for the period and duty
 * cycle can be seen below.
 *
 *                MSP430i2041
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST           P1.5|--->75% Duty Cycle PWM
 *            |                  |
 *            |                  |
 *            |                  |
 *
 *
 * Author: Zack Lalanne
 ******************************************************************************/
#include "driverlib.h"

// 8ms Period Calculation:
// SMCLK = DCO = 16.384 MHz
// TACLK = SMCLK / 4 = 4.096 MHz
// 8ms / (1 / 4.096MHz) = 32768
#define TIMER_PERIOD        32768

// 75% Duty Cycle Calculation:
// TIMER_PERIOD * 0.75 = 24576
#define TIMER_DUTY_CYCLE    24576

int main(void) {

    Timer_A_outputPWMParam outputPWMConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_4,      // SMCLK/4 = 4.096 MHz
        TIMER_PERIOD,                       // 8ms
        TIMER_A_CAPTURECOMPARE_REGISTER_1,  // Output on TA0.1
        TIMER_A_OUTPUTMODE_RESET_SET,       // Generate PWM
        TIMER_DUTY_CYCLE                    // 75% Duty Cycle
    };

    WDT_hold(WDT_BASE);

    // Setting the DCO to use the internal resistor. DCO will be at 16.384MHz
    // SMCLK = DCO = 16.384MHz
    CS_setupDCO(CS_INTERNAL_RESISTOR);
    CS_initClockSignal(CS_SMCLK, CS_CLOCK_DIVIDER_1);

    // Setting up P1.5 as TA0.1 for PWM signal generation
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1,
                                                GPIO_PIN5,
                                                GPIO_SECONDARY_MODULE_FUNCTION);

    // Configure the timer to use ACLK and interrupt on overflow
    Timer_A_outputPWM(TIMER_A0_BASE, &outputPWMConfig);

    // Go into LPM0 with interrupts enabled
    __bis_SR_register(LPM0_bits);
}

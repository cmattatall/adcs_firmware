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
 * MSP430i2xx Timer_A - Period Capture
 *
 * Description: In this example, the 16 bit Timer_A is configured to capture
 * the period of an incoming signal. For this example ACLK is looped back to
 * the Timer_A capture pin to provide an example source. When the input clock
 * signal goes from low to high the capture interrupt fires and the current
 * value of the timer is stored in an array for data logging. To determine the
 * period of the clock source simply subtract two adjacent capture values to
 * get the number of ticks between their rising edges.
 *
 * In our example the input clock source is ACLK running at 32kHz. Timer_A is
 * using SMCLK for sampling the data which runs at 16.384MHz or 61.04ns. The
 * difference between samples is 512 meaning the input clock source has a
 * period of 512 * 61.04ns = 31.25us or 32kHz. Once enough data captures have
 * occurred the device blinks the LED indicating the data is ready.
 *
 *                MSP430i2041
 *             ------------------
 *         /|\|                  |
 *          | |              P1.4|--->LED
 *          --|RST               |
 *            |         P1.5/CC1A|<---|
 *            |                  |    |
 *            |         P1.2/ACLK|--->|
 *            |                  |
 *
 *
 * Author: Zack Lalanne
 ******************************************************************************/
#include "driverlib.h"

#define NUMBER_OF_CAPTURES  50

uint16_t CaptureData[NUMBER_OF_CAPTURES];
uint16_t CaptureDataIndex = 0;

int main(void) {

    Timer_A_initCaptureModeParam captureModeConfig = {
        TIMER_A_CAPTURECOMPARE_REGISTER_1,        // CC Register 1
        TIMER_A_CAPTUREMODE_RISING_EDGE,          // Rising Edge
        TIMER_A_CAPTURE_INPUTSELECT_CCIxA,        // CCIxA Input Select
        TIMER_A_CAPTURE_SYNCHRONOUS,              // Synchronized Capture
        TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,  // Enable interrupt
        TIMER_A_OUTPUTMODE_OUTBITVALUE            // Output bit value
    };

    Timer_A_initContinuousModeParam continuousModeConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_1,      // SMCLK = 16.384MHz
        TIMER_A_TAIE_INTERRUPT_DISABLE,     // Disable Overflow ISR
        TIMER_A_DO_CLEAR                    // Clear Counter
    };


    WDT_hold(WDT_BASE);

    // Setting the DCO to use the internal resistor. DCO will be at 16.384MHz
    // SMCLK = DCO = 16.384MHz
    // ACLK = 32kHz
    CS_setupDCO(CS_INTERNAL_RESISTOR);
    CS_initClockSignal(CS_SMCLK, CS_CLOCK_DIVIDER_1);

    // Set P1.2 to output ACLK
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1,
                                                GPIO_PIN2,
                                                GPIO_SECONDARY_MODULE_FUNCTION);

    // Set P1.5 to perform period measurement as CC1A
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                                               GPIO_PIN5,
                                               GPIO_SECONDARY_MODULE_FUNCTION);


    // Configure the timer to capture on rising edge of CC1A (P1.5)
    Timer_A_initCaptureMode(TIMER_A0_BASE, &captureModeConfig);

    // Configure the sampling timer
    Timer_A_initContinuousMode(TIMER_A0_BASE, &continuousModeConfig);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);

    // Go into LPM0 with interrupts enabled. Don't exit until samples are done
    __bis_SR_register(LPM0_bits | GIE);

    // All samples recorded, blink LED to indicate done
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN4);

    // Slow down clock so we can see LED blink
    // Configure MCLK = ~1MHz
    CS_initClockSignal(CS_MCLK, CS_CLOCK_DIVIDER_16);

    while(1) {
        // Toggle the LED state
        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN4);
        __delay_cycles(100000);
    }
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
        case TA0IV_TACCR1:
            CaptureData[CaptureDataIndex++] = Timer_A_getCaptureCompareCount(TIMER_A0_BASE,
                                                                             TIMER_A_CAPTURECOMPARE_REGISTER_1);
            if(CaptureDataIndex == NUMBER_OF_CAPTURES) {
                // Enough samples, can disable interrupt
                Timer_A_disableCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);

                // Exit from LPM0 and disable interrupts
                __bic_SR_register_on_exit(LPM0_bits | GIE);
            }
            break;
        case TA0IV_TACCR2: break;
        case TA0IV_TAIFG: break;
        default:   break;
    }
}

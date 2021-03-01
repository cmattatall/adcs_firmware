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
//!
//! ICC - ICC, Nested interrupt with software configured priority
//!
//! Use Interrupt Compare Controller (ICC) to show nested interrupt
//! with software configured priority. There are 3 interrupts used
//! in this code. Set P4 interrupt highest priority. Set P2 interrupt
//! lowest priority. Set RTC interrupt medium priority. Use 3 LEDs
//! to indicate the execution of 3 interrupts. When RTC interrupt is
//! being served, P4 interrupt can preempt RTC interrupt but P2 interrupt
//! can only be served after RTC interrupt execution is finished.
//! Press Switch1(S1) and Switch2(S2) and then watch the LEDs status.
//!
//! RTC Clock = MCLK = REFO = 32kHz, MCLK = SMCLK = default DCODIV = ~1MHz.
//!
//!           MSP430FR2xx_4xx Board
//!              ---------------
//!          /|\|               |
//!           | |           LED1|-->RTC interrupt (Medium priority)
//!           --|RST   LED1&LED2|-->Port4 interrupt (Highest priority)
//!             |           LED2|-->Port2 interrupt (Lowest priority)
//!             |               |
//!             |               |<--S1
//!             |               |<--S2
//!
//! This example uses the following peripherals and I/O signals. You must
//! review these and change as needed for your own board:
//! - ICC peripheral
//! - GPIO Port peripheral
//! - RTC peripheral
//!
//! This example uses the following interrupt handlers. To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - PORT4_VECTOR
//! - PORT2_VECTOR
//! - RTC_VECTOR
//!
//!  Wallace Tran
//!  Texas Instruments Inc.
//!  May 2018
//!
//******************************************************************************
#include "driverlib.h"
#include "Board.h"

void main (void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);

    //Set MCLK output (optional for debugging)
    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_MCLK,
            GPIO_PIN_MCLK,
            GPIO_FUNCTION_MCLK);
    //REFO sources MCLK
    CS_initClockSignal(
            CS_MCLK,
            CS_REFOCLK_SELECT,
            CS_CLOCK_DIVIDER_1);
    //Clear all OSC fault flags
    CS_clearAllOscFlagsWithTimeout(1000);

    //Set S1 and S2 as input pins pulled-up.
    GPIO_setAsInputPinWithPullUpResistor(
            GPIO_PORT_S1 | GPIO_PORT_S2,
            GPIO_PIN_S1 | GPIO_PIN_S2);

    //Set LED1 and LED2 as output pins.
    //Clear LED1 and LED2 output latch for a defined power-on state
    GPIO_setOutputLowOnPin(
            GPIO_PORT_LED1 | GPIO_PORT_LED2,
            GPIO_PIN_LED1 | GPIO_PIN_LED2);
    GPIO_setAsOutputPin(
            GPIO_PORT_LED1 | GPIO_PORT_LED2,
            GPIO_PIN_LED1 | GPIO_PIN_LED2);

    /*
     * S1 and S2
     * Set hi/low edge
     * Interrupt enable
     */
    GPIO_selectInterruptEdge(
            GPIO_PORT_S1 | GPIO_PORT_S2,
            GPIO_PIN_S1 | GPIO_PIN_S2,
            GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_enableInterrupt(
            GPIO_PORT_S1 | GPIO_PORT_S2,
            GPIO_PIN_S1 | GPIO_PIN_S2);

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    /*
     * Clear S1 and S2 flag
     */
    GPIO_clearInterrupt(
            GPIO_PORT_S1 | GPIO_PORT_S2,
            GPIO_PIN_S1 | GPIO_PIN_S2);

    /*
     * Initialize RTC
     * RTC count reload compare value at 256.
     * 1024/32768 * 256 = 8 seconds
     * Source = 32kHz crystal, divided by 1024
     */
    RTC_init(
            RTC_BASE,
            256-1,
            RTC_CLOCKPREDIVIDER_1024);
    RTC_enableInterrupt(
            RTC_BASE,
            RTC_OVERFLOW_INTERRUPT);
    RTC_start(
            RTC_BASE,
            RTC_CLOCKSOURCE_XT1CLK);

    /*
     * Initialize and enable ICC
     */
    //Disable global interupt during setup
    __bic_SR_register(GIE);

    //NOTE: ICC_LEVEL_0 is the highest priority, ICC_LEVEL3 is the lowest priority
    //Set P4 interrupt highest priority
    ICC_setInterruptLevel(ICC_ILSR_P4, ICC_LEVEL_0);
    //Set P2 interrupt lowest priority
    ICC_setInterruptLevel(ICC_ILSR_P2, ICC_LEVEL_3);
    //Set RTC interrupt medium priority
    ICC_setInterruptLevel(ICC_ILSR_RTC_COUNTER, ICC_LEVEL_1);

    //Enable ICC module
    ICC_enable();

    //Enter LPM3 and global interrupt
    __bis_SR_register(LPM3_bits + GIE);
}

// Port 4 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT4_VECTOR))) Port_4 (void)
#else
#error Compiler not supported!
#endif
{
    //Clear S1 IFG
    GPIO_clearInterrupt(
            GPIO_PORT_S1,
            GPIO_PIN_S1);
    //Set LED1 and LED2 to indicate port interrupt
    GPIO_setOutputHighOnPin(
            GPIO_PORT_LED1 | GPIO_PORT_LED2,
            GPIO_PIN_LED1 | GPIO_PIN_LED2);
    // Keep LED1 and LED2 output high for 0.5 second
    __delay_cycles(500000);
    //Clear LED1 and LED2 output
    GPIO_setOutputLowOnPin(
            GPIO_PORT_LED1 | GPIO_PORT_LED2,
            GPIO_PIN_LED1 | GPIO_PIN_LED2);
}

// Port 2 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT2_VECTOR))) Port_2 (void)
#else
#error Compiler not supported!
#endif
{
    //Clear S2 IFG
    GPIO_clearInterrupt(
            GPIO_PORT_S2,
            GPIO_PIN_S2);
    //Set LED2 to indicate port interrupt
    GPIO_setOutputHighOnPin(
            GPIO_PORT_LED2,
            GPIO_PIN_LED2);
    // Keep LED2 output high for 0.5 second
    __delay_cycles(500000);
    //Clear LED2 output
    GPIO_setOutputLowOnPin(
            GPIO_PORT_LED2,
            GPIO_PIN_LED2);
}

// RTC interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=RTC_VECTOR
__interrupt void RTC_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(RTC_VECTOR))) RTC_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(RTCIV,RTCIV_RTCIF))
    {
        case  RTCIV_NONE:   break;          // No interrupt
        case  RTCIV_RTCIF:                  // RTC Overflow
                            //Enable global interrupt
                            __bis_SR_register(GIE);
                            //Set LED1 to indicate RTC interrupt
                            GPIO_setOutputHighOnPin(
                                    GPIO_PORT_LED1,
                                    GPIO_PIN_LED1);
                            //Keep LED1 output high for 4 seconds
                            __delay_cycles(4000000);
                            //Clear LED1 output
                            GPIO_setOutputLowOnPin(
                                    GPIO_PORT_LED1,
                                    GPIO_PIN_LED1);
                            break;
        default: break;
    }
}


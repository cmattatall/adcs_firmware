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
//   hspll_ex1_output_ussxtosc.c - Output USSXT OSC at 8MHz on a pin.
//
//   Description: Configure USSXT = 8MHz and output on a pin. This can be used
//   for monitoring the clock or use the clock as the source of another subsystem.
//
//           MSP430FR6047
//         ---------------
//     /|\|               |
//      | |               |
//      --|RST            |
//        |           P1.0|---> LED
//        |               |-USSXTIN
//        |               |-USSXTOUT
//        |           P8.7|---> USSXT_BOUT (8MHz)
//
//   Wallace Tran
//   Texas Instruments Inc.
//   June 2017
//******************************************************************************
#include "driverlib.h"

void main(void)
{
    // Stop WDT
    WDT_A_hold(WDT_A_BASE);

    // Configure P8.7 as USSXT_BOUT
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_P8,
        GPIO_PIN7,
        GPIO_SECONDARY_MODULE_FUNCTION
    );

    // Configure P1.0 as output for LED
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN0,
        GPIO_PRIMARY_MODULE_FUNCTION
    );

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PMM_unlockLPM5();

    // Clock System Setup
    // Set DCO to 8MHz
    CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_3);
    // Set SMCLK = MCLK = DCO, ACLK = VLOCLK
    // MCLK = SMCLK = 1MHz
    CS_initClockSignal(CS_ACLK, CS_VLOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_8);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_8);
    
    // Configure USSXT Oscillator
    HSPLL_xtalInitParam param = {0};
    param.oscillatorType = HSPLL_XTAL_OSCTYPE_XTAL;
    param.oscillatorEnable = HSPLL_XTAL_ENABLE;
    param.xtlOutput = HSPLL_XTAL_OUTPUT_ENABLE;
    HSPLL_xtalInit(HSPLL_BASE, &param);
    
    // Check if oscillator is stable
    while(HSPLL_getOscillatorStatus(HSPLL_BASE) == HSPLL_OSCILLATOR_NOT_STABILIZED);
    
    // Set up timer to wait in LPM for crystal stabilization time = 4096 clocks for crystal resonator.
    // For 8MHz XTAL, 4096 clocks = 512us. Using VLO = 9.4kHz, wait 5 timer clock cycles = 532us.
    Timer_A_initUpModeParam timerParam = {0};
    timerParam.timerPeriod = 5;
    timerParam.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE;
    // Timer sourced from ACLK (VLO)
    timerParam.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
    // Clear timer
    timerParam.timerClear = TIMER_A_DO_CLEAR;
    timerParam.startTimer = true;
    Timer_A_initUpMode(TA4_BASE, &timerParam);

    // Enter LPM3 w/interrupts enabled
    __bis_SR_register(LPM3_bits | GIE);

    // For debugger
    __no_operation();
    
    while(1)
    {
        // Toggle LED to show oscillator init complete
        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
        // Wait 100,000 CPU cycles
        __delay_cycles(50000);
    }
}

// Timer A4 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER4_A0_VECTOR
__interrupt void Timer4_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER4_A0_VECTOR))) Timer4_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    // Stop the timer and wake up from LPM
    Timer_A_startCounter(TA4_BASE, TIMER_A_STOP_MODE);
    __bic_SR_register_on_exit(LPM3_bits | GIE);
    __no_operation();
}

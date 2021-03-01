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
//   saph_ex1_pulseGeneration.c - Pulse generation using the SAPH PPG and PHY.
//
//   Description: Configure the PPG and PHY in the SAPH module manually (register
//   mode instead of ASQ controlled) to generate 10 high frequency pulses on the
//   dedicated CH0_OUT pin, once per second.
//
//           MSP430FR6047
//         ---------------
//     /|\|               |
//      | |               |
//      --|RST            |
//        |           P1.0|---> LED
//        |               |-USSXTIN
//        |               |-USSXTOUT
//        |        CH0_OUT|---> 10 pulses 1/sec at 10MHz
//
//   Wallace Tran
//   Texas Instruments Inc.
//   June 2017
//   Built with IAR Embedded Workbench V6.50 & Code Composer Studio V7.1
//******************************************************************************
#include "driverlib.h"

void main(void)
{
    // Stop WDT
    WDT_A_hold(WDT_A_BASE);

    // Configure P1.0 as output for LED
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN0,
        GPIO_PRIMARY_MODULE_FUNCTION
    );

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PMM_unlockLPM5();

        /* ---Begin Clock System Setup--- */
    // Set DCO to 8MHz
    CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_3);
    // Set SMCLK = MCLK = DCO, ACLK = VLOCLK
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_8);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_8);
    CS_initClockSignal(CS_ACLK, CS_VLOCLK_SELECT, CS_CLOCK_DIVIDER_1);
        /* ---End Clock System Setup--- */
    
        /* ---Begin initialize the HSPLL and wait for it to lock--- */
    // Configure USSXT Oscillator
    HSPLL_xtalInitParam xtalParam = {0};
    xtalParam.oscillatorType = HSPLL_XTAL_OSCTYPE_XTAL;
    xtalParam.oscillatorEnable = HSPLL_XTAL_ENABLE;
    xtalParam.xtlOutput = HSPLL_XTAL_OUTPUT_ENABLE;
    HSPLL_xtalInit(HSPLL_BASE, &xtalParam);
    
    // Check if oscillator is stable
    while(HSPLL_getOscillatorStatus(HSPLL_BASE) == HSPLL_OSCILLATOR_NOT_STABILIZED);
    
    // Set up timer to wait in LPM for crystal stabilization time = 4096 clocks for crystal resonator.
    // For 8MHz XTAL, 4096 clocks = 512us. Using VLO = 9.4kHz, wait 5 timer clock cycles = 532us.
    Timer_A_initUpModeParam timerParam = {0};
    timerParam.timerPeriod = 5;
    timerParam.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE;
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
    
    // Init PLL
    // Use the PLL multiplier setting to get 80MHz output from our 8MHz input
    // Equation: PLL output clock frequency x 2 = input clock frequency x (hspllParam.multiplier+1)
    // Input clock frequency = 8MHz
    // Desired PLL output clock frequency = 80MHz
    // hspllParam.multipler = 19
    HSPLL_initParam hspllParam = {0};
    hspllParam.multiplier = PLLM_19_H;
    hspllParam.frequency = HSPLL_GREATER_THAN_6MHZ;
    HSPLL_init(HSPLL_BASE, &hspllParam);
    
    // Power up the UUPS to start the PLL
    UUPS_turnOnPower(UUPS_BASE, UUPS_POWERUP_TRIGGER_SOURCE_USSPWRUP);
    
    // Wait for UUPS to power up
    while(UUPS_getPowerModeStatus(UUPS_BASE) != UUPS_POWERMODE_READY);
    
    // Wait for PLL to lock
    while(HSPLL_isLocked(HSPLL_BASE) == HSPLL_UNLOCKED);
    
        /* ---End initialize the HSPLL and wait for it to lock--- */
    
        /* ---Begin setting up the PPG settings--- */
    // 10 excitation pulses, 0 stop pulses, output low when inactive, high polarity
    SAPH_unlock(SAPH_BASE);
    SAPH_configPPGCountParam saphCountConfig = {0};
    saphCountConfig.pauseLevel = SAPH_PPG_PAUSE_LEVEL_LOW;
    saphCountConfig.pausePolarity = SAPH_PPG_PAUSE_POLARITY_HIGH;
    saphCountConfig.excitationPulseCount = 0x000A;
    SAPH_configurePPGCount(SAPH_BASE, &saphCountConfig);
    // Low phase = 4 HSPLL cycles = 50ns
    SAPH_setPPGLowPeriod(SAPH_BASE, 4);
    // High phase = 4 HSPLL cycles = 50ns
    SAPH_setPPGHighPeriod(SAPH_BASE, 4);
    // Set up the PHY to output PPG on dedicated CH0_OUT pin
    SAPH_configPHYParam saphPHYConfig = {0};
    // Output PPG
    saphPHYConfig.outputFunction = SAPH_PHY_OUTPUT_PULSEGENERATOR_SINGLE_DRIVE;
    SAPH_configurePHY(SAPH_BASE, &saphPHYConfig);
    // Enable the PPG
    // TA2.1 trigger, CH0 output, register mode
    SAPH_configPPGParam saphConfig = {0};
    saphConfig.triggerSource = SAPH_PPG_TRIGGER_SOURCE_TIMER;
    saphConfig.channelSelect = SAPH_PPG_CHANNEL_0;
    saphConfig.portSelect = SAPH_PPG_PORT_CHARGED_BY_PPG;
    SAPH_configurePPG(SAPH_BASE, &saphConfig);
        /* ---End setting up the PPG settings--- */
    
        /* ---Begin configure TA2.1 for 1/sec to trigger the pulse generation and toggle LED--- */
    Timer_A_initUpModeParam timerParam2 = {0};
    timerParam2.timerPeriod = 9400;
    TA2CCR1 = 4700;
    TA2CCTL1 = OUTMOD_7 | CCIE;     // Enable output signal to trigger PPG, enable interrupt
    // Timer sourced from ACLK (VLO)
    timerParam2.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
    // Clear timer
    timerParam2.timerClear = TIMER_A_DO_CLEAR;
    timerParam2.startTimer = true;
    Timer_A_initUpMode(TA2_BASE, &timerParam2);
        /* ---End configure TA2.1 for 1/sec to trigger the pulse generation and toggle LED--- */
        
    while(1)
    {
        // Enter LPM0 w/interrupt
        __bis_SR_register(LPM0_bits | GIE);
        // For debugger
        __no_operation();
    }
}

// Timer A2 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER2_A1_VECTOR
__interrupt void Timer2_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER2_A1_VECTOR))) Timer2_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(TA2IV, TAIV__TAIFG))
    {
        case TAIV__NONE:   break;               // No interrupt
        case TAIV__TACCR1:
            GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);    // Toggle LED to show new cycle
            break;
        case TAIV__TAIFG: break;                // overflow
        default: break;
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

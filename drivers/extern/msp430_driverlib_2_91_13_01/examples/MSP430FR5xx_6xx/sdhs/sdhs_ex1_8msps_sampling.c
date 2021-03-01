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
//   sdhs_ex1_8msps_sampling.c - SDHS sampling at 8MSPS and DTC transfer results to RAM
//
//   Description: Configure the SDHS for stand-alone use (register mode) at 8MSPS.
//   Use the DTC to transfer the results to an array in RAM.
//
//           MSP430FR6047
//         ---------------
//     /|\|               |
//      | |               |
//      --|RST            |
//        |           P1.0|---> LED
//        |               |-USSXTIN
//        |               |-USSXTOUT
//        |         CH0_IN|<--- input signal
//
//   Wallace Tran
//   Texas Instruments Inc.
//   June 2017
//******************************************************************************
#include "driverlib.h"

#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(results, ".leaRAM")
#pragma RETAIN(results)
uint16_t results[1024] = {0};
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma location = 0x2C00
__no_init uint16_t results[1024];
#pragma required = results
#elif defined(__GNUC__)
uint16_t results[1024] __attribute__ ((section(".leaRAM"), used));
#else
#error Compiler not supported!
#endif

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

    // Configure one FRAM waitstate as required by the device datasheet for MCLK
    FRAMCtl_A_configureWaitStateControl(FRAMCTL_A_ACCESS_TIME_CYCLES_1);

    /* ---Begin Clock System Setup--- */
    // Per Device Errata set divider to 4 before changing frequency to
    // prevent out of spec operation from overshoot transient
    CS_initClockSignal(CS_ACLK, CS_VLOCLK_SELECT, CS_CLOCK_DIVIDER_4);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_4);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_4);
    // Set DCO to 16MHz
    CS_setDCOFreq(CS_DCORSEL_1, CS_DCOFSEL_4);
    // Delay by ~10us to let DCO settle. 60 cycles = 20 cycles buffer + (10us / (1/4MHz))
    __delay_cycles(60);
    // Set all dividers to 1 for 16MHz operation
    // MCLK = SMCLK = 16MHz
    CS_initClockSignal(CS_ACLK, CS_VLOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    /* ---End Clock System Setup--- */

    /* ---Begin Setup SAPH Bias Register Controlled--- */
    SAPH_unlock(SAPH_BASE);
    SAPH_configPHYBiasParam saphParam = {0};
    saphParam.enableChargePump = SAPH_PHY_MULTIPLEXER_CHARGEPUMP_ENABLE;
    saphParam.biasSwitchASQ = SAPH_PHY_BIAS_SWITCH_CONTROLLED_BY_REGISTER;
    saphParam.biasPGA = SAPH_PHY_PGA_BIAS_NOMINAL_VALUE;
    saphParam.biasExcitation = SAPH_PHY_EXCITATION_BIAS_NOMINAL_VALUE;
    SAPH_configurePHYBias(SAPH_BASE, &saphParam);
    SAPH_configurePHYMultiplexer(SAPH_BASE, SAPH_PHY_DUMMYLOAD_DISABLE,
                                 SAPH_PHY_SOURCE_CONTROLLED_BY_REGISTER,
                                 SAPH_PHY_INPUT_CHANNEL_0);
    SAPH_lock(SAPH_BASE);
    /* ---End Setup SAPH Bias Register Controlled--- */

    /* ---Begin initialize the HSPLL and wait for it to lock--- */
    // Configure USSXT Oscillator
    HSPLL_xtalInitParam xtalParam;
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
    HSPLL_initParam hspllParam;
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

    /* ---Begin setup the SDHS with register mode--- */
    SDHS_initParam sdhsParam = {0};
    // Software trigger
    sdhsParam.triggerSourceSelect = SDHS_REGISTER_CONTROL_MODE;
    // Automatic sample start after settling delay
    sdhsParam.autoSampleStart = SDHS_AUTO_SAMPLE_START_DISABLED;
    sdhsParam.dataAlignment = SDHS_DATA_FORMAT_OFFSET_BINARY;
    sdhsParam.dataFormat = SDHS_DATA_ALIGNED_LEFT;
    // OSR = 10 (80MHz/10 = 8MSPS). CS must be set MCLK >= 8 MHz.
    sdhsParam.oversamplingRate = SDHS_OVERSAMPLING_RATE_10;
    SDHS_init(SDHS_BASE, &sdhsParam);
    // 256 samples
    SDHS_setTotalSampleSize(SDHS_BASE, 256);
    // PGA Gain 0.1 dB
    SDHS_setPGAGain(SDHS_BASE, 0x1A);
    // 80MHz PLL output frequency
    SDHS_setModularOptimization(SDHS_BASE, SDHS_OPTIMIZE_PLL_OUTPUT_FREQUENCY_77_80MHz);
    // DTC transfer data to start of LEA RAM
    SDHS_setDTCDestinationAddress(SDHS_BASE, 0);
    // Enable acquisition done interrupt (after 256 samples transferred)
    SDHS_enableInterrupt(SDHS_BASE, SDHS_ACQUISITION_DONE_INTERRUPT);
    // Enable trigger
    SDHS_enableTrigger(SDHS_BASE);
    // Turn on SD and start conversion
    SDHS_enable(SDHS_BASE);
    SDHS_startConversion(SDHS_BASE);
    // Delay for PGA worst case 40us settling time
    __delay_cycles(320);
    /* ---End setup the SDHS with register mode--- */

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
void __attribute__ ((interrupt(TIMER2_A1_VECTOR))) Timer2_A1_ISR(void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(TA2IV, TAIV__TAIFG))
    {
    case TAIV__NONE:   break;                   // No interrupt
    case TAIV__TACCR1:
        SDHS_endConversion(SDHS_BASE);
        SDHS_startConversion(SDHS_BASE);       // Start conversion
        break;
    case TAIV__TAIFG: break;                    // overflow
    default: break;
    }
}

// SDHS interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = SDHS_VECTOR
__interrupt void SDHS_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(SDHS_VECTOR))) SDHS_ISR(void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(SDHSIIDX, IIDX_6))
    {
    case IIDX_0:   break;                   // No interrupt
    case IIDX_1:   break;                   // OVF interrupt
    case IIDX_2:                            // ACQDONE interrupt
        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);     // Toggle LED to show new cycle
        __delay_cycles(10000);
        __no_operation();                   //put breakpoint here to view results
        break;
    case IIDX_3:   break;                   // SSTRG interrupt
    case IIDX_4:   break;                   // DTRDY interrupt
    case IIDX_5:   break;                   // WINHI interrupt
    case IIDX_6:   break;                   // WINLO interrupt
    default: break;
    }
}

// Timer A4 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER4_A0_VECTOR
__interrupt void Timer4_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER4_A0_VECTOR))) Timer4_A0_ISR(void)
#else
#error Compiler not supported!
#endif
{
    // Stop the timer and wake up from LPM
    Timer_A_startCounter(TA4_BASE, TIMER_A_STOP_MODE);
    __bic_SR_register_on_exit(LPM3_bits | GIE);
    __no_operation();
}

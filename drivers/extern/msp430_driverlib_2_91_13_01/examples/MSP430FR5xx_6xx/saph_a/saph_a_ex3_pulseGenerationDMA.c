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
//   saph_a_ex3_pulseGenerationDMA.c - Pulse generation using the SAPH_A PPG/PHY and DMA.
//
//   Description: Configure the PPG and PHY in the SAPH_A module manually (register
//   mode instead of ASQ controlled) to generate 6(Can be more) sets of high frequency
//   pulses on the dedicated CH0_OUT pin in Multi Tone mode, once per second.
//   6 sets of pulses(Can be defined by users):
//          Pulses number: 2-4-3-3-4-4
//          Pulses frequency(KHz): 625-1250-277-625-178-384
//   ***NOTE: For the limitation of the DMA transmission speed, the minimum duration
//            time of every set of pulses should be more than 4*(3+9) MCLK.***
//
//          MSP430FR6043
//         ---------------
//     /|\|               |
//      | |               |
//      --|RST            |
//        |           P1.0|---> LED
//        |               |-USSXTIN
//        |               |-USSXTOUT
//        |        CH0_OUT|---> 6 sets of pulses 1/sec
//
//   Wallace Tran
//   Texas Instruments Inc.
//   January 2019
//   Built with IAR Embedded Workbench V7.10 & Code Composer Studio V7.3
//******************************************************************************
#include "driverlib.h"

uint16_t RAM_XE[20]; // => 3 E-puls phases x 3 Register + 2 X-puls phases x 3 register+3 STOP
uint16_t RAM_DMA_CONFIG[60];    // => (3 E-puls +  2 X-puls + 1 STOP)*9
//The address of RAM_DMA_CONFIG[] should be within 0xFFFF to make the minimum number of transmit data by DMA3 which is 9
uint16_t epulse[3] = {4,3,4};     // E-pulses number
uint16_t hper[3]   = {8,16,26};   // High phase
uint16_t lper[4]   = {8,16,26};   // Low phase

uint16_t xpulse[2] = {3,4};       // X-pulses number
uint16_t xhper[2]  = {36,56};     // High phase
uint16_t xlper[2]  = {36,56};     // Low phase

void HSPLL_routine(void);
void setup_DMA();

int main(void)
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

    FRAMCtl_A_configureWaitStateControl(FRAMCTL_A_ACCESS_TIME_CYCLES_1);

        /* ---Begin Clock System Setup--- */
    // Per Device Errata(CS12) set divider to 4 before changing frequency to
    // prevent out of spec operation from overshoot transient
    // Set all corresponding clk sources to divide by 4 for errata(CS12)
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_4);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_4);
    CS_initClockSignal(CS_ACLK, CS_VLOCLK_SELECT, CS_CLOCK_DIVIDER_4);
    // Set DCO to 8MHz
    CS_setDCOFreq(CS_DCORSEL_1, CS_DCOFSEL_4);
    // Delay by ~10us to let DCO settle. 60 cycles = 20 cycles buffer + (10us / (1/4MHz))
    __delay_cycles(60);
    // Set SMCLK = MCLK = DCO, ACLK = VLOCLK
    // Set all dividers to 1 for MCLK = SMCLK = 16MHz operation
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_ACLK, CS_VLOCLK_SELECT, CS_CLOCK_DIVIDER_1);
        /* ---End Clock System Setup--- */

    HSPLL_routine();                           // Initialize the HSPLL and wait for it to lock

    setup_DMA();                            // Initialize the DMA

        /* ---Begin setting up the PPG settings--- */
    // Unlock the SAPH_A registers
    SAPH_A_unlock(SAPH_A_BASE);

    // Set up the PHY to output PPG on dedicated CH0_OUT pin
    SAPH_A_configPHYParam saphAPHYConfig = {0};
    saphAPHYConfig.outputFunction = SAPH_A_PHY_OUTPUT_PULSEGENERATOR_SINGLE_DRIVE;
    SAPH_A_configurePHY(SAPH_A_BASE, &saphAPHYConfig);

    // TA2.1 trigger, CH0 output, register mode
    SAPH_A_configPPGParam saphAConfig = {0};
    saphAConfig.enablePrescaler = SAPH_A_PPG_PRESCALER_ENABLE;
    saphAConfig.triggerSource = SAPH_A_PPG_TRIGGER_SOURCE_TIMER;
    saphAConfig.channelSelect = SAPH_A_PPG_CHANNEL_0;
    saphAConfig.portSelect = SAPH_A_PPG_PORT_CHARGED_BY_PPG;
    SAPH_A_configurePPG(SAPH_A_BASE, &saphAConfig);
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
       __bis_SR_register(LPM0_bits | GIE);  // Enter LPM0 enable interrupt
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
    SAPH_A_configXPGParam saphAXPGParam = {0};
    DMA_initParam dmaParam = {0};
    switch(__even_in_range(TA2IV, TAIV__TAIFG))
    {
        case TAIV__NONE:   break;           // No interrupt
        case TAIV__TACCR1:
            saphAXPGParam.eventType = SAPH_A_DMA_TRIGGER_EVENT;
            saphAXPGParam.extendedMode = SAPH_A_DUAL_TONE_LOOP;
            saphAXPGParam.extraPulses = 2;
            SAPH_A_configureExtendedPPG(SAPH_A_BASE, &saphAXPGParam);

            SAPH_A_setXPGLowPeriod(SAPH_A_BASE, 16);
            SAPH_A_setXPGHighPeriod(SAPH_A_BASE, 16);

            // Repeated block transfer, increase src address, increase dst address
            dmaParam.channelSelect = DMA_CHANNEL_2;
            dmaParam.transferModeSelect = DMA_TRANSFER_REPEATED_BLOCK;
            DMA_init(&dmaParam);
            DMA_setSrcAddress(DMA_CHANNEL_2, (uint32_t)(uintptr_t)&RAM_XE, DMA_DIRECTION_INCREMENT);
            DMA_setDstAddress(DMA_CHANNEL_2, (uint32_t)(uintptr_t)&SAPHPGC, DMA_DIRECTION_INCREMENT);

            DMA_setTransferSize(DMA_CHANNEL_2, 3);  // 3 registers per block
            DMA_enableTransfers(DMA_CHANNEL_2);
            // Write new SA and DA of DMA2
            DMA_setSrcAddress(DMA_CHANNEL_2, (uint32_t)(uintptr_t)RAM_XE+3, DMA_DIRECTION_INCREMENT);
            DMA_setDstAddress(DMA_CHANNEL_2, (uint32_t)(uintptr_t)&SAPHXPGCTL, DMA_DIRECTION_INCREMENT);

            // Repeated block transfer, increase src address, increase dst address
            dmaParam.channelSelect = DMA_CHANNEL_3;
            DMA_init(&dmaParam);
            DMA_setSrcAddress(DMA_CHANNEL_3, (uint32_t)(uintptr_t)&RAM_DMA_CONFIG, DMA_DIRECTION_INCREMENT);
            DMA_setDstAddress(DMA_CHANNEL_3, (uint32_t)(uintptr_t)&DMA2SA, DMA_DIRECTION_INCREMENT);

            DMA_setTransferSize(DMA_CHANNEL_3, 9);  // 10 DMA registers
            DMA_enableTransfers(DMA_CHANNEL_3);
            // Write new SA of DMA3
            DMA_setSrcAddress(DMA_CHANNEL_3, (uint32_t)(uintptr_t)RAM_DMA_CONFIG+9, DMA_DIRECTION_INCREMENT);

            GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);    // Toggle LED to show new cycle
            break;
        case TAIV__TAIFG: break;            // overflow
        default: break;
    }
}

void HSPLL_routine(void)
{
        /* ---Begin initialize the HSPLL and wait for it to lock--- */
    // Configure USSXT Oscillator
    HSPLL_xtalInitParam param = {0};
//    param.oscillatorType = HSPLL_XTAL_OSCTYPE_XTAL;
    param.oscillatorType = 0x0000;
    param.oscillatorEnable = HSPLL_XTAL_ENABLE;
    param.xtlOutput = HSPLL_XTAL_OUTPUT_ENABLE;
    HSPLL_xtalInit(HSPLL_BASE, &param);

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

    // Check if oscillator is stable
    while(HSPLL_getOscillatorStatus(HSPLL_BASE) == HSPLL_OSCILLATOR_NOT_STARTED);

    // Output oscillator on pin
    HSPLLUSSXTLCTL &= ~XTOUTOFF;

    // Init PLL
    // Use the PLL multiplier setting to get 80MHz output from our 8MHz input
    // Equation: PLL output clock frequency x 2 = input clock frequency x (hspllParam.multiplier+1)
    // Input clock frequency = 8MHz
    // Desired PLL output clock frequency = 80MHz
    // hspllParam.multipler = 19
    HSPLL_initParam hspllParam = {0};
//    hspllParam.multiplier = PLLM_19_H;
    hspllParam.multiplier = 0x004c;
    hspllParam.frequency = HSPLL_GREATER_THAN_6MHZ;
    HSPLL_init(HSPLL_BASE, &hspllParam);

    // Power up the UUPS to start the PLL
    UUPS_turnOnPower(UUPS_BASE, UUPS_POWERUP_TRIGGER_SOURCE_USSPWRUP);

    // Wait for UUPS to power up
    while(UUPS_getPowerModeStatus(UUPS_BASE) != UUPS_POWERMODE_READY);

    // Wait for PLL to lock
    while(HSPLL_isLocked(HSPLL_BASE) == HSPLL_UNLOCKED);
        /* ---End initialize the HSPLL and wait for it to lock--- */
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
    // Stop the timer and wake from LPM
    Timer_A_stop(TA4_BASE);
    __bic_SR_register_on_exit(LPM3_bits | GIE);
    __no_operation();
}

void setup_DMA()
{

    int i;
    unsigned long SAPHPGC_addr, SAPHXPGCTL_addr;
    SAPHPGC_addr = (unsigned long)(uintptr_t)(&SAPHPGC);
    SAPHXPGCTL_addr = (unsigned long)(uintptr_t)(&SAPHXPGCTL);
    // DMA2 and DMA3 triggered by PPG, DMA2 has higher priority
    DMACTL1 = DMA2TSEL_27|DMA3TSEL_27;

    SAPH_A_configXPGParam saphAXPGParam = {0};
    saphAXPGParam.eventType = SAPH_A_DMA_TRIGGER_EVENT;
    saphAXPGParam.extendedMode = SAPH_A_DUAL_TONE_LOOP;
    saphAXPGParam.extraPulses = 2;
    SAPH_A_configureExtendedPPG(SAPH_A_BASE, &saphAXPGParam);

    SAPH_A_setXPGLowPeriod(SAPH_A_BASE, 16);
    SAPH_A_setXPGHighPeriod(SAPH_A_BASE, 16);

    // Repeated block transfer, increase src address, increase dst address
    DMA_initParam dmaParam = {0};
    dmaParam.channelSelect = DMA_CHANNEL_2;
    dmaParam.transferModeSelect = DMA_TRANSFER_REPEATED_BLOCK;
    DMA_init(&dmaParam);
    DMA_setSrcAddress(DMA_CHANNEL_2, (uint32_t)(uintptr_t)&RAM_XE, DMA_DIRECTION_INCREMENT);
    DMA_setDstAddress(DMA_CHANNEL_2, (uint32_t)(uintptr_t)&SAPHPGC, DMA_DIRECTION_INCREMENT);

    DMA_setTransferSize(DMA_CHANNEL_2, 3);  // 3 registers per block
    DMA_enableTransfers(DMA_CHANNEL_2);
    // Write new SA and DA of DMA2
    DMA_setSrcAddress(DMA_CHANNEL_2, (uint32_t)(uintptr_t)RAM_XE+3, DMA_DIRECTION_INCREMENT);
    DMA_setDstAddress(DMA_CHANNEL_2, (uint32_t)(uintptr_t)&SAPHXPGCTL, DMA_DIRECTION_INCREMENT);

    // Repeated block transfer, increase src address, increase dst address
    dmaParam.channelSelect = DMA_CHANNEL_3;
    DMA_init(&dmaParam);
    DMA_setSrcAddress(DMA_CHANNEL_3, (uint32_t)(uintptr_t)&RAM_DMA_CONFIG, DMA_DIRECTION_INCREMENT);
    DMA_setDstAddress(DMA_CHANNEL_3, (uint32_t)(uintptr_t)&DMA2SA, DMA_DIRECTION_INCREMENT);

    DMA_setTransferSize(DMA_CHANNEL_3, 9);  // 10 DMA registers
    DMA_enableTransfers(DMA_CHANNEL_3);
    // Write new SA of DMA3
    DMA_setSrcAddress(DMA_CHANNEL_3, (uint32_t)(uintptr_t)RAM_DMA_CONFIG+9, DMA_DIRECTION_INCREMENT);

    // Setup RAM
    // RAM to be transfered to DMA2 and DMA3                              // DST Addresses
    for (i = 0; i < 5; i++)
    {
        *(RAM_DMA_CONFIG+i*9)   = (unsigned int)((unsigned long)(uintptr_t)(RAM_XE+(i+2)*3));// DMA2SA
        *(RAM_DMA_CONFIG+1+i*9) = (unsigned int)(((unsigned long)(uintptr_t)(RAM_XE+(i+2)*3))>>16);// DMA2SAH
        *(RAM_DMA_CONFIG+2+i*9) = (unsigned int)(SAPHPGC_addr);            // DMA2DA
        *(RAM_DMA_CONFIG+3+i*9) = 0x0000;                                  // DMA2DAH
        *(RAM_DMA_CONFIG+4+i*9) = 0x0003;                                  // DMA2SZ
        *(RAM_DMA_CONFIG+5+i*9) = 0x0000;                                  // dummy
        *(RAM_DMA_CONFIG+6+i*9) = 0x0000;                                  // dummy
        *(RAM_DMA_CONFIG+7+i*9) = 0x5000|DMASRCINCR|DMADSTINCR|DMAEN;      // DMA3CTL
        *(RAM_DMA_CONFIG+8+i*9) = (unsigned int)(unsigned long)(uintptr_t)(RAM_DMA_CONFIG+(i+2)*9);  // DMA3SA
    }
    for (i = 0; i < 2; i++)
    {
        *(RAM_DMA_CONFIG + 11+i*18) = (unsigned int)(SAPHXPGCTL_addr);      // DMA2DA
    }

    // RAM to be transfered to XCTL or PGC
    for (i = 0; i < 3; i++)
    {
        *(RAM_XE+i*6) = epulse[i];       // SPULSE EPULSE SAPHPGC
        *(RAM_XE+1+i*6) = lper[i];       // SAPHLPGPER
        *(RAM_XE+2+i*6) = hper[i];       // SAPHHPGPER
    }
    for (i = 0; i < 2; i++)
    {
        *(RAM_XE+3+i*6) = 0x7000|xpulse[i];// DMA_EN XMOD XPULSE SAPHXPGCTL
        *(RAM_XE+4+i*6) = xlper[i];      // SAPHAXLPGPER
        *(RAM_XE+5+i*6) = xhper[i];      // SAPHAXHPGPER
    }

    // Stop
    *(RAM_XE+15) = 0x6000;              // XMOD 2
    *(RAM_XE+15+1) = 0x0000;
    *(RAM_XE+15+2) = 0x0000;
}


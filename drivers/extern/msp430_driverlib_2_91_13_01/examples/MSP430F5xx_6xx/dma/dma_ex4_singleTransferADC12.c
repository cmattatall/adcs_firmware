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
//! DMA - Single Transfer using ADC12 trigger by TIMER_A, on Channel 0
//!
//!  This software uses TBCCR1 as a sample and convert input into the A0 of
//!  ADC12. ADC12IFG is used to trigger a DMA transfer and DMA interrupt
//!  triggers when DMA transfer is done. TB1 is set as an output and P1.0 is
//!  toggled when DMA ISR is serviced.
//!  ACLK = REFO = 32kHz, MCLK = SMCLK = default DCO 1048576Hz
//!
//!                 MSP430F552x
//!             -----------------
//!         /|\|              XIN|-
//!          | |                 | 32kHz
//!          --|RST          XOUT|-
//!            |                 |
//!            |             P1.0|-> LED
//!            |         P5.7/TB1|-> TIMER_A_B1 output
//!            |          P6.0/A0|<- A0
//!            |                 |
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - DMA peripheral
//! - ADC12 peripheral
//! - GPIO Port peripheral
//! - A0
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - DMA_VECTOR
//!
//******************************************************************************

#include "driverlib.h"

//ADC conversion result is stored in this variable
uint16_t DMA_DST;

void main (void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);

    //Clear P1.0 LED off
    GPIO_setOutputLowOnPin(
        GPIO_PORT_P1,
        GPIO_PIN0
        );

    //Set P1.0 to output direction
    GPIO_setAsOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN0
        );

    //P5.7/TB1 option select Output direction
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_P5,
        GPIO_PIN7
        );
    //Enable A/D channel A0
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P6,
        GPIO_PIN0
        );

    //Set up Timer A
    Timer_A_initCompareModeParam initCompParam = {0};
    initCompParam.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    initCompParam.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initCompParam.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    initCompParam.compareValue = 0x8000;
    Timer_A_initCompareMode(TIMER_A0_BASE, &initCompParam);
	
	//Start timer A0
	Timer_A_initUpDownModeParam initUpDownParam = {0};
    initUpDownParam.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    initUpDownParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    initUpDownParam.timerPeriod = 0xFFFE;
    initUpDownParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    initUpDownParam.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    initUpDownParam.timerClear = TIMER_A_DO_CLEAR;
    initUpDownParam.startTimer = true;
    Timer_A_initUpDownMode(TIMER_A0_BASE, &initUpDownParam);

	//Initialize and setup the ADC12 Module
    /*
     * Base Address of the ADC12 Module
     * Use internal ADC12 bit as sample/hold signal to start conversion
     * USE MODOSC 5MHZ Digital Oscillator as clock source
     * Use default clock divider of 1
     */
    ADC12_A_init(ADC12_A_BASE,
        ADC12_A_SAMPLEHOLDSOURCE_3,
        ADC12_A_CLOCKSOURCE_ADC12OSC,
        ADC12_A_CLOCKDIVIDER_1);

    ADC12_A_enable(ADC12_A_BASE);
    /*
     * Base Address of the ADC12 Module
     * For memory buffers 0-7 sample/hold for 256 clock cycles
     * For memory buffers 8-15 sample/hold for 4 clock cycles (default)
     * Enable Multiple Sampling
     */
    ADC12_A_setupSamplingTimer(ADC12_A_BASE,
        ADC12_A_CYCLEHOLD_128_CYCLES,
        ADC12_A_CYCLEHOLD_4_CYCLES,
        ADC12_A_MULTIPLESAMPLESENABLE);

    //Configure Memory Buffer
    /*
     * Base Address of the ADC12 Module
     * Configure memory buffer 0
     * Map input A0 to memory buffer 0
     * Vref+ = AVcc
     * Vref- = AVss
     * Memory buffer 0 is not the end of a sequence
     */
    ADC12_A_configureMemoryParam configureMemoryParam = {0};
	configureMemoryParam.memoryBufferControlIndex = ADC12_A_MEMORY_0;
	configureMemoryParam.inputSourceSelect = ADC12_A_INPUT_A0;
	configureMemoryParam.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_AVCC;
	configureMemoryParam.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
	configureMemoryParam.endOfSequence = ADC12_A_NOTENDOFSEQUENCE;
	ADC12_A_configureMemory(ADC12_A_BASE ,&configureMemoryParam);
	

    //Enable/Start sampling and conversion
    /*
     * Base Address of the ADC12 Module
     * Start the conversion into memory buffer 0
     * Use the single-channel, single-conversion mode
     */
    ADC12_A_startConversion(ADC12_A_BASE,
        ADC12_A_MEMORY_0,
        ADC12_A_REPEATED_SINGLECHANNEL);

    //Disable the DMA from preempting a Read-Modify-Write Operation in the CPU
    DMA_disableTransferDuringReadModifyWrite();

    //Initialize and Setup DMA Channel 0
    /*
     * Configure DMA channel 0
     * Configure channel for repeated single transfers
     * DMA interrupt flag will be set after every 1 transfer
     * Use DMA Trigger Source 24 (ADC12xIFG)
     * Transfer Word-to-Word
     * Trigger upon Rising Edge of Trigger Source Signal
     */
    DMA_initParam initParam = {0};
    initParam.channelSelect = DMA_CHANNEL_0;
    initParam.transferModeSelect = DMA_TRANSFER_REPEATED_SINGLE;
    initParam.transferSize = 1;
    initParam.triggerSourceSelect = DMA_TRIGGERSOURCE_24;
    initParam.transferUnitSelect = DMA_SIZE_SRCWORD_DSTWORD;
    initParam.triggerTypeSelect = DMA_TRIGGER_RISINGEDGE;
    DMA_init(&initParam);
    /*
     * Configure DMA channel 0
     * Use ADC12 Memory Buffer 0 as source
     * Don't move the source address after every transfer
     */
    DMA_setSrcAddress(DMA_CHANNEL_0,
        ADC12_A_getMemoryAddressForDMA(ADC12_A_BASE,
            ADC12_A_MEMORY_0),
        DMA_DIRECTION_UNCHANGED);
    /*
     * Configure DMA channel 0
     * Use DMA_DST as destination
     * Increment destination address after every transfer
     */
    DMA_setDstAddress(DMA_CHANNEL_0,
        (uint32_t)(uintptr_t)&DMA_DST,
        DMA_DIRECTION_INCREMENT);

    //Enable DMA channel 0 interrupt
	DMA_clearInterrupt(DMA_CHANNEL_0);
    DMA_enableInterrupt(DMA_CHANNEL_0);

    //Enable transfers on DMA channel 0
    DMA_enableTransfers(DMA_CHANNEL_0);

    //LPM0 w/ interrupts
    __bis_SR_register(LPM0_bits + GIE);
    //For Debugger
    __no_operation();
}

//------------------------------------------------------------------------------
//DMA Interrupt Service Routine
//------------------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=DMA_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(DMA_VECTOR)))
#endif
void DMA_ISR (void)
{
    switch (__even_in_range(DMAIV,16)){
        case 0: break;
        case 2:         //DMA0IFG = DMA Channel 0
            //Toggle P1.0 - PLACE BREAKPOINT HERE AND CHECK DMA_DST VARIABLE
            GPIO_toggleOutputOnPin(
				GPIO_PORT_P1,
				GPIO_PIN0
				);
            break;
        case 4: break;  //DMA1IFG = DMA Channel 1
        case 6: break;  //DMA2IFG = DMA Channel 2
        case 8: break;  //DMA3IFG = DMA Channel 3
        case 10: break; //DMA4IFG = DMA Channel 4
        case 12: break; //DMA5IFG = DMA Channel 5
        case 14: break; //DMA6IFG = DMA Channel 6
        case 16: break; //DMA7IFG = DMA Channel 7
        default: break;
    }
}


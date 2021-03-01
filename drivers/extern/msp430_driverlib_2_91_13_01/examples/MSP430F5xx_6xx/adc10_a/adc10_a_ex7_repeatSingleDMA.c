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
#include "driverlib.h"

//******************************************************************************
//!
//! ADC10_A - Sample A1 Input, DMA 64 Samples, Repeat Single
//!
//!  MSP430F550x Demo
//!  Sample A1 64 times with AVcc ref. Software writes to ADC10SC to trigger
//!  sample burst. In Mainloop MSP430 waits in LPM0 to save power until ADC10_A
//!  conversion burst complete, DMA_IFG will force exit from LPM0 in Mainloop
//!  on reti. ADC10_A internal oscillator times sample period (16x) and conversion
//!  (13x). DMA transfers conversion result in ADC10MEM0 to array ADC_Result.
//!  P1.0 set at start of conversion burst, reset on completion.
//!
//!                MSP430F550x
//!             -----------------
//!         /|\|              XIN|-
//!          | |                 |
//!          --|RST          XOUT|-
//!            |          P6.1/A1|<- Sample Signal
//!            |                 |
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - ADC10_A peripheral
//! - DMA peripheral
//! - GPIO Port peripheral
//! - A1
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - DMA_VECTOR
//!
//******************************************************************************

void main (void)
{
	uint16_t ADC_Result[64];
	volatile uint16_t ADC_Result_Average;
    uint8_t i;
    uint16_t ADC_Result_sum;

    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);

    //Initialize the ADC10_A Module
    /*
     * Base Address for the ADC10_A Module
     * Use internal ADC10_A bit as sample/hold signal to start conversion
     * USE MODOSC 5MHZ Digital Oscillator as clock source
     * Use default clock divider of 1
     */
    ADC10_A_init(ADC10_A_BASE,
        ADC10_A_SAMPLEHOLDSOURCE_SC,
        ADC10_A_CLOCKSOURCE_ADC10OSC,
        ADC10_A_CLOCKDIVIDER_1);

    ADC10_A_enable(ADC10_A_BASE);

    /*
     * Base Address for the ADC10_A Module
     * Sample/hold for 16 clock cycles
     * Enable Multiple Sampling
     */
    ADC10_A_setupSamplingTimer(ADC10_A_BASE,
        ADC10_A_CYCLEHOLD_16_CYCLES,
        ADC10_A_MULTIPLESAMPLESENABLE);

    //Configure Memory Buffer
    /*
     * Base Address for the ADC10_A Module
     * Use input A1
     * Use positive reference of AVcc
     * Use negative reference of AVss
     */
    ADC10_A_configureMemory(ADC10_A_BASE,
        ADC10_A_INPUT_A1,
        ADC10_A_VREFPOS_AVCC,
        ADC10_A_VREFNEG_AVSS);

    //Initialize and Setup DMA Channel 0
    /*
     * Configure DMA channel 0
     * Configure channel for repeated single transfer
     * DMA interrupt flag will be set after every 64 transfers
     * Use DMA Trigger Source 24 (ADC10IFG)
     * Transfer Word-to-Word
     * Trigger upon Rising Edge of Trigger Source Signal
     */
   	DMA_initParam param = {0};
    param.channelSelect = DMA_CHANNEL_0;
    param.transferModeSelect = DMA_TRANSFER_REPEATED_SINGLE;
    param.transferSize = 64;
    param.triggerSourceSelect = DMA_TRIGGERSOURCE_24;
    param.transferUnitSelect = DMA_SIZE_SRCWORD_DSTWORD;
    param.triggerTypeSelect = DMA_TRIGGER_RISINGEDGE;
    DMA_init(&param);
    /*
     * Configure DMA channel 0
     * Use ADC10_A Memory Buffer as source
     * Increment destination address after every transfer
     */
    DMA_setSrcAddress(DMA_CHANNEL_0,
        ADC10_A_getMemoryAddressForDMA(ADC10_A_BASE),
        DMA_DIRECTION_UNCHANGED);
    /*
     * Base Address for the DMA Module
     * Configure DMA channel 0
     * Use ADC_Result[0] as destination
     * Increment destination address after every transfer
     */
    DMA_setDstAddress(DMA_CHANNEL_0,
        (uint32_t)(uintptr_t)&ADC_Result[0],
        DMA_DIRECTION_INCREMENT);

    //Enable DMA channel 0 interrupt
	DMA_clearInterrupt(DMA_CHANNEL_0);
    DMA_enableInterrupt(DMA_CHANNEL_0);

    //Enable transfers on DMA channel 0
    DMA_enableTransfers(DMA_CHANNEL_0);

    while (1)
    {
        //Enable and Start the conversion
        //in Repeated Single-Channel Conversion Mode
        ADC10_A_startConversion(ADC10_A_BASE,
            ADC10_A_REPEATED_SINGLECHANNEL);

        __bis_SR_register(CPUOFF + GIE);        //LPM0, ADC10_A_ISR will force exit
        __no_operation();                       //For debug only

        //Clear accumulate register
        ADC_Result_sum = 0x0;
        for (i = 0; i < 64; i++){
            ADC_Result_sum += ADC_Result[i];
        }

        //Average of 64 conversions results
        ADC_Result_Average = ADC_Result_sum >> 6;

        //SET BREAKPOINT HERE to be able to watch ADC_Result_Average
        //Delay before next 64 conversions
        __delay_cycles(50000);
    }
}


// DMA interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(DMA_VECTOR)))
#endif
void DMA0_ISR (void) {
    switch (__even_in_range(DMAIV,16)){
        case  0: break; //No interrupt
        case  2:        //DMA0IFG
            //64 conversions complete
            //Disable Conversions without pre-empting any conversions taking place.
            ADC10_A_disableConversions(ADC10_A_BASE,
            ADC10_A_PREEMPTCONVERSION);
            //Exit LPM
            __bic_SR_register_on_exit(CPUOFF);
            break;
        case  4: break; //DMA1IFG
        case  6: break; //DMA2IFG
        default: break;
    }
}

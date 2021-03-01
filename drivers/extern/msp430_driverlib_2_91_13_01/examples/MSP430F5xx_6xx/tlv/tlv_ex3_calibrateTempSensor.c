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
//! TLV - Sample Calibrated A10 Internal Temp Sensor as Input, AVcc Ref
//!
//!  After initialization, the the calibration constants for the
//!  unit are read in through the TLV. A single sample is made on A10 with
//!  reference to internal 1.5V Vref. Software sets ADC12SC to start sample and
//!  conversion - ADC12SC automatically cleared at EOC. ADC12 internal
//!  oscillator times sample and conversion. In Mainloop MSP430 waits in LPM4
//!  to save power until ADC10 conversion complete, ADC12_A_ISR will force exit
//!  from any LPMx in Mainloop on reti.
//!
//!
//!  The results of the temperature sense can be viewed in the watch window by
//!  watching the variables DegC and DegF
//!
//!  ACLK = n/a, MCLK = SMCLK = default DCO ~ 1.045MHz, ADC12CLK = ADC12OSC
//!
//!
//!                MSP430F5438A
//!             -----------------
//!         /|\|                 |
//!          | |                 |
//!          --|RST              |
//!            |Temp Sensor-->A10|
//!            |                 |
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - ADC12 peripheral
//! - A10
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - ADC12_A_VECTOR
//!
//******************************************************************************

#include "driverlib.h"

uint16_t temp;
volatile float DegF;
volatile float DegC;
float mref, nref;
uint8_t bADCCAL_bytes;
struct s_TLV_ADC_Cal_Data * pADCCAL;

//******************************************************************************
//
// This function will initialize and calibrate ADC for temperature sensing.
// The embedded function call of Get_TLV_Info searches the TLV block of data
// (defined in the device header file) for the ADC calibration block, then
// returns the address of the first data descriptor to the pointer passed into
// it. The slope and offset of the corrected temperature curve are then
// calculated for temperature calibration purposes.
//
//******************************************************************************
void calibrateADC(void);

void main (void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);

    calibrateADC();

    while (1)
    {
        //Enable/Start first sampling and conversion cycle
        /*
         * Base address of ADC12 Module
         * Start the conversion into memory buffer 0
         * Use the repeated sequence of channels
         */
        ADC12_A_startConversion(ADC12_A_BASE,
            ADC12_A_MEMORY_0,
            ADC12_A_SEQOFCHANNELS);

        //LPM0 with interrupts enabled
        __bis_SR_register(LPM4_bits + GIE);
        __no_operation();

        //Temperature in Celsius
        DegC = (temp - nref) / mref;

        //Temperature in Fahrenheit
        DegF = (9*DegC)/5 + 32;

        //SET BREAKPOINT HERE and watch IntDegC and IntDegF
        __no_operation();
    }
}

void calibrateADC(void)
{
  //Initialize the ADC12 Module
    /*
     * Base Address of ADC12 Module
     * Use internal ADC12 bit as sample/hold signal to start conversion
     * USE MODOSC 5MHZ Digital Oscillator as clock source
     * Use default clock divider of 1
     */
    ADC12_A_init(ADC12_A_BASE,
        ADC12_A_SAMPLEHOLDSOURCE_SC,
        ADC12_A_CLOCKSOURCE_ADC12OSC,
        ADC12_A_CLOCKDIVIDER_1);

    ADC12_A_enable(ADC12_A_BASE);

    /*
     * Base Address of ADC12 Module
     * For memory buffers 0-7 sample/hold for 256 clock cycles
     * For memory buffers 8-15 sample/hold for 4 clock cycles (default)
     * Disable Multiple Sampling
     */
    ADC12_A_setupSamplingTimer(ADC12_A_BASE,
        ADC12_A_CYCLEHOLD_256_CYCLES,
        ADC12_A_CYCLEHOLD_4_CYCLES,
        ADC12_A_MULTIPLESAMPLESDISABLE);

    //Configure Memory Buffer
    /*
     * Base Addres of ADC12 Module
     * Configure memory buffer 0
     * Map temp sensor to memory buffer 0
     * Vref+ = Vref+ (int)
     * Vref- = AVss
     * Memory buffer 0 is not the end of a sequence
     */
   	ADC12_A_configureMemoryParam param = {0};
	param.memoryBufferControlIndex = ADC12_A_MEMORY_0;
	param.inputSourceSelect = ADC12_A_INPUT_TEMPSENSOR;
	param.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_INT;
	param.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
	param.endOfSequence = ADC12_A_NOTENDOFSEQUENCE;
	ADC12_A_configureMemory(ADC12_A_BASE ,&param);
	

    //Enable memory buffer 0 interrupt
	ADC12_A_clearInterrupt(ADC12_A_BASE,
		ADC12IFG0);
    ADC12_A_enableInterrupt(ADC12_A_BASE,
        ADC12IE0);

    //Configure internal reference
    //If ref generator busy, WAIT
    while ( REF_ACTIVE == Ref_isRefGenBusy(REF_BASE) ) ;
    //Select internal ref = 1.5V
    Ref_setReferenceVoltage(REF_BASE,
        REF_VREF1_5V);
    //Internal Reference ON
    Ref_enableReferenceVoltage(REF_BASE);

    //Delay (~75us) for Ref to settle
    __delay_cycles(75);

    // TLV access Function Call
    TLV_getInfo(TLV_TAG_ADCCAL,
                0,
                &bADCCAL_bytes,
                (uint16_t **)&pADCCAL
                );

    mref = ((float)
            (pADCCAL->adc_ref15_85_temp-pADCCAL->adc_ref15_30_temp))/(85-30);
    nref = pADCCAL->adc_ref15_85_temp-mref*85;
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC12_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(ADC12_VECTOR)))
#endif
void ADC12ISR (void)
{
    switch (__even_in_range(ADC12IV,34)){
        case  0: break;   //Vector  0:  No interrupt
        case  2: break;   //Vector  2:  ADC overflow
        case  4: break;   //Vector  4:  ADC timing overflow
        case  6:          //Vector  6:  ADC12IFG0
            //Move results, IFG is cleared
            temp = ADC12_A_getResults(ADC12_A_BASE,
            ADC12_A_MEMORY_0);

            //Exit active CPU
            __bic_SR_register_on_exit(LPM4_bits);
            break;
        case  8: break;   //Vector  8:  ADC12IFG1
        case 10: break;   //Vector 10:  ADC12IFG2
        case 12: break;   //Vector 12:  ADC12IFG3
        case 14: break;   //Vector 14:  ADC12IFG4
        case 16: break;   //Vector 16:  ADC12IFG5
        case 18: break;   //Vector 18:  ADC12IFG6
        case 20: break;   //Vector 20:  ADC12IFG7
        case 22: break;   //Vector 22:  ADC12IFG8
        case 24: break;   //Vector 24:  ADC12IFG9
        case 26: break;   //Vector 26:  ADC12IFG10
        case 28: break;   //Vector 28:  ADC12IFG11
        case 30: break;   //Vector 30:  ADC12IFG12
        case 32: break;   //Vector 32:  ADC12IFG13
        case 34: break;   //Vector 34:  ADC12IFG14
        default: break;
    }
}


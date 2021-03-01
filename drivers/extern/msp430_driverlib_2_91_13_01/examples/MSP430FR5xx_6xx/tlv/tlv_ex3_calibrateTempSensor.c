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
//! TLV - Sample Calibrated A30 Internal Temp Sensor as Input, AVcc Ref
//!
//!  After initialization, the the calibration constants for the
//!  unit are read in through the TLV. A single sample is made on A30 with
//!  reference to internal 1.2V Vref. Software sets ADC12SC to start sample and
//!  conversion - ADC12SC automatically cleared at EOC. ADC12 internal
//!  oscillator times sample and conversion. In Mainloop MSP430 waits in LPM4
//!  to save power until ADC12 conversion complete, ADC12_B_ISR will force exit
//!  from any LPMx in Mainloop on reti.
//!
//!
//!  The results of the temperature sense can be viewed in the watch window by
//!  watching the variables DegC and DegF
//!
//!  ACLK = n/a, MCLK = SMCLK = default DCO ~ 1.045MHz, ADC12CLK = ADC12OSC
//!
//!
//!                MSP430FR5969
//!             -----------------
//!         /|\|                 |
//!          | |                 |
//!          --|RST              |
//!            |Temp Sensor-->A30|
//!            |                 |
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - ADC12 peripheral
//! - A30(TempSensor)
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - ADC12_B_VECTOR
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
        ADC12_B_startConversion(ADC12_B_BASE,
            ADC12_B_MEMORY_0,
            ADC12_B_SINGLECHANNEL);

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
    //Initialize the ADC12B Module
    /*
    * Base address of ADC12B Module
    * Use internal ADC12B bit as sample/hold signal to start conversion
    * USE MODOSC 5MHZ Digital Oscillator as clock source
    * Use default clock divider/pre-divider of 1
    * Not use internal channel
    */
    ADC12_B_initParam initParam = {0};
    initParam.sampleHoldSignalSourceSelect = ADC12_B_SAMPLEHOLDSOURCE_SC;
    initParam.clockSourceSelect = ADC12_B_CLOCKSOURCE_ADC12OSC;
    initParam.clockSourceDivider = ADC12_B_CLOCKDIVIDER_1;
    initParam.clockSourcePredivider = ADC12_B_CLOCKPREDIVIDER__1;
    initParam.internalChannelMap = ADC12_B_NOINTCH;
    ADC12_B_init(ADC12_B_BASE, &initParam);

    //Enable the ADC12B module
    ADC12_B_enable(ADC12_B_BASE);

    /*
    * Base address of ADC12B Module
    * For memory buffers 0-7 sample/hold for 64 clock cycles
    * For memory buffers 8-15 sample/hold for 4 clock cycles (default)
    * Disable Multiple Sampling
    */
    ADC12_B_setupSamplingTimer(ADC12_B_BASE,
      ADC12_B_CYCLEHOLD_64_CYCLES,
      ADC12_B_CYCLEHOLD_4_CYCLES,
      ADC12_B_MULTIPLESAMPLESDISABLE);

    //Configure Memory Buffer
    /*
    * Base address of the ADC12B Module
    * Configure memory buffer 0
    * Map input Temperature Sensor to memory buffer 0
    * Vref+ = VREF buffered
    * Vref- = AVss
    * Memory buffer 0 is not the end of a sequence
    */
    ADC12_B_configureMemoryParam configureMemoryParam = {0};
    configureMemoryParam.memoryBufferControlIndex = ADC12_B_MEMORY_0;
    configureMemoryParam.inputSourceSelect = ADC12_B_INPUT_TCMAP;
    configureMemoryParam.refVoltageSourceSelect = ADC12_B_VREFPOS_INTBUF_VREFNEG_VSS;
    configureMemoryParam.endOfSequence = ADC12_B_NOTENDOFSEQUENCE;
    configureMemoryParam.windowComparatorSelect = ADC12_B_WINDOW_COMPARATOR_DISABLE;
    configureMemoryParam.differentialModeSelect = ADC12_B_DIFFERENTIAL_MODE_DISABLE;
    ADC12_B_configureMemory(ADC12_B_BASE, &configureMemoryParam);

    ADC12_B_clearInterrupt(ADC12_B_BASE,
        0,
        ADC12_B_IFG0
        );

    //Enable memory buffer 0 interrupt
    ADC12_B_enableInterrupt(ADC12_B_BASE,
      ADC12_B_IE0,
      0,
      0);

    // If ref generator busy, WAIT
    while(REF_A_BUSY == Ref_A_isRefGenBusy(REF_A_BASE));
    // Select internal ref = 1.2V
    Ref_A_setReferenceVoltage(REF_A_BASE, REF_A_VREF1_2V);
    // Internal Reference ON
    Ref_A_enableReferenceVoltage(REF_A_BASE);
    // Wait for reference generator to settle
    while(REF_A_READY != Ref_A_isVariableReferenceVoltageOutputReady(REF_A_BASE));

    // TLV access Function Call
    TLV_getInfo(TLV_TAG_ADCCAL,
                0,
                &bADCCAL_bytes,
                (uint16_t **)&pADCCAL
                );

    mref = ((float)
            (pADCCAL->adc_ref15_85_temp - pADCCAL->adc_ref15_30_temp))/(85-30);
    nref = pADCCAL->adc_ref15_85_temp - mref*85;
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC12_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(ADC12_VECTOR)))
#endif
void ADC12ISR (void)
{
    switch(__even_in_range(ADC12IV,12)) {
        case  0: break;                         // Vector  0:  No interrupt
        case  2: break;                         // Vector  2:  ADC12BMEMx Overflow
        case  4: break;                         // Vector  4:  Conversion time overflow
        case  6: break;                         // Vector  6:  ADC12BHI
        case  8: break;                         // Vector  8:  ADC12BLO
        case 10: break;                         // Vector 10:  ADC12BIN
        case 12:                                // Vector 12:  ADC12BMEM0 Interrupt
            //Move results, IFG is cleared
            temp = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_0);

            //Exit active CPU
            __bic_SR_register_on_exit(LPM4_bits);
            break;
        case 14: break;                         // Vector 14:  ADC12BMEM1
        case 16: break;                         // Vector 16:  ADC12BMEM2
        case 18: break;                         // Vector 18:  ADC12BMEM3
        case 20: break;                         // Vector 20:  ADC12BMEM4
        case 22: break;                         // Vector 22:  ADC12BMEM5
        case 24: break;                         // Vector 24:  ADC12BMEM6
        case 26: break;                         // Vector 26:  ADC12BMEM7
        case 28: break;                         // Vector 28:  ADC12BMEM8
        case 30: break;                         // Vector 30:  ADC12BMEM9
        case 32: break;                         // Vector 32:  ADC12BMEM10
        case 34: break;                         // Vector 34:  ADC12BMEM11
        case 36: break;                         // Vector 36:  ADC12BMEM12
        case 38: break;                         // Vector 38:  ADC12BMEM13
        case 40: break;                         // Vector 40:  ADC12BMEM14
        case 42: break;                         // Vector 42:  ADC12BMEM15
        case 44: break;                         // Vector 44:  ADC12BMEM16
        case 46: break;                         // Vector 46:  ADC12BMEM17
        case 48: break;                         // Vector 48:  ADC12BMEM18
        case 50: break;                         // Vector 50:  ADC12BMEM19
        case 52: break;                         // Vector 52:  ADC12BMEM20
        case 54: break;                         // Vector 54:  ADC12BMEM21
        case 56: break;                         // Vector 56:  ADC12BMEM22
        case 58: break;                         // Vector 58:  ADC12BMEM23
        case 60: break;                         // Vector 60:  ADC12BMEM24
        case 62: break;                         // Vector 62:  ADC12BMEM25
        case 64: break;                         // Vector 64:  ADC12BMEM26
        case 66: break;                         // Vector 66:  ADC12BMEM27
        case 68: break;                         // Vector 68:  ADC12BMEM28
        case 70: break;                         // Vector 70:  ADC12BMEM29
        case 72: break;                         // Vector 72:  ADC12BMEM30
        case 74: break;                         // Vector 74:  ADC12BMEM31
        case 76: break;                         // Vector 76:  ADC12BRDY
        default: break;
    }
}

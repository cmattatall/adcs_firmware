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
//   MSP430FR60xx Demo - Pulse generation using the MTIF
//
//   Description: Configure the MTIF for pulse generation. 8 pulses per 250ms.
//   Configure the MTIF to also count the pulses - should get the same number
//   generated.
//
//           MSP430FR6047
//         ---------------
//     /|\|               |
//      | |               |
//      --|RST            |
//        |           P1.0|---> LED
//        |           PJ.4|-LFXIN
//        |           PJ.5|-LFXOUT
//        |    MTIF_OUT_IN|<--> 8 pulses per 250ms
//
//   Wallace Tran
//   Texas Instruments Inc.
//   June 2017
//   Built with IAR Embedded Workbench V6.50 & Code Composer Studio V7.1
//******************************************************************************
#include "driverlib.h"

volatile uint32_t pulseCount[10] = {0};
uint8_t i = 0;

void main (void)
{
    // Stop WDT
    WDT_A_hold(WDT_A_BASE);
    
    // Configure P1.0 as output for LED
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN0,
        GPIO_PRIMARY_MODULE_FUNCTION
    );
    
    // Configure the PJ.4 & PJ.5 for LFXTIN/LFXTOUT
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_PJ,
        GPIO_PIN4 + GPIO_PIN5,
        GPIO_PRIMARY_MODULE_FUNCTION
    );

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PMM_unlockLPM5();
    
        /* ---Begin initialize the clock system--- */
    // Set DCO to 8MHz
    CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_3);
    // Set ACLK = LFXTCLK, SMCLK = MCLK = DCO
    CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    // MCLK = SMCLK = 1 MHz
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_8);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_8);
    // Enable LFXT
    CS_turnOnLFXT(CS_LFXT_DRIVE_3);
        /* ---End initialize the clock system--- */
    
        /* ---Begin initialize the RTC--- */
    // Stop RTC
    RTC_C_holdClock(RTC_C_BASE);
    // Set Prescalar for 1Hz interval
    RTC_C_enableInterrupt(RTC_C_BASE, RTC_C_PRESCALE_TIMER1_INTERRUPT);
    RTC_C_definePrescaleEvent(RTC_C_BASE, RTC_C_PRESCALE_1, RTC_C_PSEVENTDIVIDER_128);
    // clear second and min counter
    RTC_C_setCounterValue(RTC_C_BASE, 0);
    // load PS counter
    RTC_C_setPrescaleValue(RTC_C_BASE, RTC_C_PRESCALE_0, 0);
    // clear the hold and start RTC
    RTC_C_startClock(RTC_C_BASE);
        /* ---End initialize the RTC--- */
    
        /* ---Begin initialize the MTIF--- */
    // Enable the pulse generator, and set the initial grid frequency
    // Pulse grid freq. = 1024Hz (frame freq. = 1024/256 = 4Hz)
    MTIF_setPulseGenPulseGridFreq(MTIF_BASE, MTIF_PULSE_GRID_FREQUENCY_1024HZ);
    MTIF_clearPulseGenCounter(MTIF_BASE);
    MTIF_enablePulseGen(MTIF_BASE);
    // KVAL = 4. 8 pulses per 256 periods of 1024Hz.
    MTIF_setPulseGenCountNum(MTIF_BASE, MTIF_KVAL_BIT2);
    // K-count update request
    MTIF_clearPulseKCountUpdateRequest(MTIF_BASE);
    // Wait for update acknowledge
    while(MTIF_isPulseKCountUpdated(MTIF_BASE) == MTIF_K_COUNT_IS_NOT_UPDATED);
    // Enable the MTIF terminal pins
    MTIF_enableTestPortOutput(MTIF_BASE);
    // Enable test port output
    MTIF_enableTestPortTerminalActivationBySWAndHW(MTIF_BASE);
    // Clear pulse counter and enable pulse counter
    MTIF_clearPulseCounter(MTIF_BASE);
    MTIF_enablePulseCounter(MTIF_BASE);
        /* ---End initialize the MTIF--- */
    
    while(1)
    {
        // Enter LPM3 w/interrupt
        __bis_SR_register(LPM3_bits | GIE);
        // For debug
        __no_operation();
        
        // Procedure to perform read of pulse count
        // Poll PCRA to wait for completion of previous read
        // Request pulse counter read
        MTIF_setPulseCounterReadRequest(MTIF_BASE);
        // Wait for stable read value
        while(MTIF_isPulseCounterReadReady(MTIF_BASE) == MTIF_PULSE_COUNTER_NOT_READY_TO_READ);
        // Read pulse counter value
        pulseCount[i] = MTIF_getPulseCount(MTIF_BASE);
        // Check if there was a 16-bit overflow
        if(MTIF_getPulseCounterOverflow(MTIF_BASE) == MTIF_PULSE_COUNTER_OVERFLOW)
        {
            pulseCount[i] += 0x10000;
        }
        // Put breakpoint here to observe pulse count
        __no_operation();
        i++;
        if(i == 10)
        {
            i = 0;
        }
    }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=RTC_C_VECTOR
__interrupt void RTC_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(RTC_C_VECTOR))) RTC_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(RTCIV, RTCIV__RT1PSIFG))
    {
        case RTCIV__NONE:      break;       // No interrupts
        case RTCIV__RTCOFIFG:  break;       // RTCOFIFG
        case RTCIV__RTCRDYIFG: break;       // RTCRDYIFG
        case RTCIV__RTCTEVIFG:              // RTCEVIFG

            break;
        case RTCIV__RTCAIFG:   break;       // RTCAIFG
        case RTCIV__RT0PSIFG:  break;       // RT0PSIFG
        case RTCIV__RT1PSIFG:
            GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0); // Toggle P1.0 LED
            __bic_SR_register_on_exit(LPM3_bits | GIE); //Wake to perform read
            __no_operation();               // For Debug
            break;       // RT1PSIFG
        default: break;
    }
}

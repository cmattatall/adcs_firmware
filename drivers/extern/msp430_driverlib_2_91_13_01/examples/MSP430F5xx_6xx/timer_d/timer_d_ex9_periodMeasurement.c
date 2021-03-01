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
//  MSP430F51x2 Demo - TimerD0, Dual Input capture mode, Normal timer mode,
//                                Period measurement
//  Description: This code example implements input capture in dual capture
//  mode using TimerD in normal timer mode. Dual capture mode of TimerD is
//  demonstrated here. This example captures two consequent rising edges and
//  computes the Period of the input signal. The TD0.0 input signal is generated
//  by the Timer A0 module (TA0.0) and the freq of this PWM input signal can be
//  changed by modifying the TA0CCR0 register.
//
//  ACLK = LFXT1 = 32kHz; SMCLK = MCLK = 12MHz
//
//                 MSP430F51x2
//             -----------------
//         /|\|              XIN|-
//          | |                 | 32kHz
//          --|RST          XOUT|-
//            |                 |
//            |       P1.6/TD0.0|<-- CCI0A <-|
//            |       P3.7/TA0.0|--> CCR0 -->|
//            |                 |
//            |             P3.0|--> MCLK = 12MHz DCO
//            |             P3.1|--> SMCLK = 12MHz DCO
//            |             P3.2|--> ACLK = 32kHz LFXT1
//
//******************************************************************************
#include "driverlib.h"

uint16_t REdge1, REdge2, Period;

const uint8_t port_mapping[] = {
    //Port P3:
		PM_TD0CLKMCLK,
		PM_TD0_0SMCLK,
		PM_TD1OUTH,
		PM_NONE,
		PM_NONE,
		PM_NONE,
		PM_NONE,
		PM_TA0_0,
	};


void main(void)
{
    // Stop WDT
    WDT_A_hold(WDT_A_BASE);

    // Configure XT1
    GPIO_setAsPeripheralModuleFunctionInputPin(
                		  GPIO_PORT_PJ,
                		  GPIO_PIN4 + GPIO_PIN5
                		  );

    UCS_turnOnLFXT1 (UCS_XT1_DRIVE_3,
    		  	  UCS_XCAP_3
    		  	  );


    // Increase Vcore setting to level1 to support fsystem=12MHz
    // NOTE: Change core voltage one level at a time..
    PMM_setVCore (PMM_CORE_LEVEL_1);

    // Initialize DCO to 12MHz
    UCS_initFLLSettle(12000,
    	  	  	  374);


    // Setup Port Pins
    // P3.0 - P3.2 output
    // Port map P3.0 - P3.2
    GPIO_setAsPeripheralModuleFunctionOutputPin(
                		  GPIO_PORT_P3,
                		  GPIO_PIN0 + GPIO_PIN1 + GPIO_PIN2
                		  );

    PMAP_initPortsParam initPortsParam = {0};
    initPortsParam.portMapping = (const uint8_t *)port_mapping;
    initPortsParam.PxMAPy = (uint8_t *)&P3MAP0;
    initPortsParam.numberOfPorts = 1;
    initPortsParam.portMapReconfigure = PMAP_DISABLE_RECONFIGURATION;
    PMAP_initPorts(PMAP_CTRL_BASE, &initPortsParam);


    // Configure ports TD0.0 input and TA0.0 output
    // TD0.0 input
    // TD0.0 option select
    // TA0.0 output
    // TA0.0 option select
    GPIO_setAsPeripheralModuleFunctionInputPin(
            		  GPIO_PORT_P1,
            		  GPIO_PIN6
            		  );

    GPIO_setAsPeripheralModuleFunctionOutputPin(
              		  GPIO_PORT_P3,
              		  GPIO_PIN7
              		  );

    // Configure LED on P1.0, output and driving low
    GPIO_setOutputLowOnPin(
                 		  GPIO_PORT_P1,
                 		  GPIO_PIN0
                 		  );
    GPIO_setAsOutputPin(
               		  GPIO_PORT_P1,
               		  GPIO_PIN0
               		  );

    // Configure TA0.0 compare output, 1kHz freq, 50% dutycycle
    Timer_A_initCompareModeParam initCompParam = {0};
    initCompParam.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_0;
    initCompParam.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initCompParam.compareOutputMode = TIMER_A_OUTPUTMODE_TOGGLE;
    initCompParam.compareValue = 32;
    Timer_A_initCompareMode(TIMER_A0_BASE, &initCompParam);

    Timer_A_initUpModeParam initUpParam = {0};
    initUpParam.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
    initUpParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    initUpParam.timerPeriod = 32;
    initUpParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    initUpParam.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initUpParam.timerClear = TIMER_A_DO_CLEAR;
    initUpParam.startTimer = false;
    Timer_A_initUpMode(TIMER_A0_BASE, &initUpParam);

    Timer_A_startCounter(TIMER_A0_BASE,
    	  TIMER_A_UP_MODE
    	  );

    Timer_D_initContinuousModeParam initContParam = {0};
    initContParam.clockSource = TIMER_D_CLOCKSOURCE_SMCLK;
    initContParam.clockSourceDivider = TIMER_D_CLOCKSOURCE_DIVIDER_1;
    initContParam.clockingMode = TIMER_D_CLOCKINGMODE_EXTERNAL_CLOCK;
    initContParam.timerInterruptEnable_TDIE = TIMER_D_TDIE_INTERRUPT_DISABLE;
    initContParam.timerClear = TIMER_D_DO_CLEAR;
    Timer_D_initContinuousMode(TIMER_D0_BASE, &initContParam);

    Timer_D_startCounter(TIMER_D0_BASE,
    		  TIMER_D_CONTINUOUS_MODE
    		  );

    Timer_D_clearCaptureCompareInterrupt(TIMER_D0_BASE,
        TIMER_D_CAPTURECOMPARE_REGISTER_0);

    Timer_D_initCaptureModeParam initCapParam = {0};
    initCapParam.captureRegister = TIMER_D_CAPTURECOMPARE_REGISTER_0;
    initCapParam.captureMode = TIMER_D_CAPTUREMODE_RISING_EDGE;
    initCapParam.captureInputSelect = TIMER_D_CAPTURE_INPUTSELECT_CCIxA;
    initCapParam.synchronizeCaptureSource = TIMER_D_CAPTURE_SYNCHRONOUS;
    initCapParam.captureInterruptEnable = TIMER_D_CAPTURECOMPARE_INTERRUPT_ENABLE;
    initCapParam.captureOutputMode = TIMER_D_OUTPUTMODE_OUTBITVALUE;
    initCapParam.channelCaptureMode = TIMER_D_DUAL_CAPTURE_MODE;
    Timer_D_initCaptureMode(TIMER_D0_BASE, &initCapParam);

    while(1)
    {
      __bis_SR_register(LPM0_bits + GIE);   // Enter LPM0
      __no_operation();                     // For debugger
      // On exiting LPM0
      if (TIMER_D_CAPTURE_OVERFLOW == Timer_D_getCaptureCompareInterruptStatus
          		  	  (TIMER_D0_BASE,
            	  	  	  TIMER_D_CAPTURECOMPARE_REGISTER_1,
            	  	  	  TIMER_D_CAPTURE_OVERFLOW
            	  	  	  )              )     // Check for Capture Overflow
          while(1);                         // Loop Forever

      Period = REdge2-REdge1;               // Measured Period
      __no_operation();                     // BREAKPOINT HERE - measured period ~ 2ms
                                            // Period ~ 2m*TD0 clock counts
    }
}

// Timer0_D0 Interrupt Vector
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_D0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER0_D0_VECTOR)))
#endif
void TIMER0_D0_ISR (void)
{
    REdge1 = Timer_D_getCaptureCompareLatchCount(TIMER_D0_BASE,
    	          				TIMER_D_CAPTURECOMPARE_REGISTER_0);

    REdge2 = Timer_D_getCaptureCompareCount(TIMER_D0_BASE,
          				TIMER_D_CAPTURECOMPARE_REGISTER_0);
    __bic_SR_register_on_exit(LPM0_bits);      // Exit LPM0
}



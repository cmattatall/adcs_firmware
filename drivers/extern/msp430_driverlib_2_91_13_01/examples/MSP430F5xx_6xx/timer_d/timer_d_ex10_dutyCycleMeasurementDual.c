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
//  MSP430F51x2 Demo - TimerD0, Dual Input Capture mode, Normal Timer Mode,
//                                Input Dutycycle measurement
//  Description: This code example implements input capture in dual capture
//  mode using TimerD in normal timer mode. TD1.1 is configured to output PWM
//  of 25% dutycycle, that is used as capture input to TD0.1. TD0.1 is
//  configured as timer input capture and is triggered by both the rising and
//  falling edges. An external connection between TD1.1 and TD0.1 is required.
//  Rising and Falling edges are captured in the dual capture mode and the
//  Period and Dutycycle is computed. If the measured dutycycle is != 25%, then
//  LED on P1.0 is set.
//
//  ACLK = LFXT1 = 32kHz; SMCLK = MCLK = 2.45MHz;
//
//                 MSP430F51x2
//             -----------------
//         /|\|              XIN|-
//          | |                 | 32kHz
//          --|RST          XOUT|-
//            |                 |
//            |       P1.7/TD0.1|<-- CCI1A <-|
//            |       P2.2/TD1.1|--> CCR1 -->|
//            |                 |
//            |             P1.0|--> LED "ON" if measured Dutycycle != 25%
//            |                 |
//
//******************************************************************************
#include "driverlib.h"

uint8_t Count = 0x0;
uint16_t REdge1, REdge2, FEdge1, FEdge2;
uint16_t Period, ON_Period;
uint8_t DutyCycle;

void main(void)
{
    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    // P1.0/LED Output
    // LED off
    GPIO_setAsOutputPin(
      GPIO_PORT_P1,
      GPIO_PIN0
      );

    GPIO_setOutputLowOnPin(
      GPIO_PORT_P1,
      GPIO_PIN0
      );

    // Configure XT1
    // Port select XT1
    GPIO_setAsPeripheralModuleFunctionInputPin(
    	            		  GPIO_PORT_PJ,
    	            		  GPIO_PIN4 + GPIO_PIN5
    	            		  );

    UCS_turnOnLFXT1(UCS_XT1_DRIVE_3,
    		  UCS_XCAP_3);

    // Initialize DCO to 2.45MHz
    // (74 + 1) * 32768 = 2.45MHz
    UCS_initFLLSettle(2450,
    	  	  	  74);


    // Configure Port Pins
    GPIO_setAsPeripheralModuleFunctionInputPin(
              		  GPIO_PORT_P1,
              		  GPIO_PIN7
              		  );

    GPIO_setAsPeripheralModuleFunctionOutputPin(
                		  GPIO_PORT_P2,
                		  GPIO_PIN2
                		  );

    // Configure TD1.1 to output PWM signal
    Timer_D_initCompareModeParam initCompParam = {0};
    initCompParam.compareRegister = TIMER_D_CAPTURECOMPARE_REGISTER_1;
    initCompParam.compareInterruptEnable = TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initCompParam.compareOutputMode = TIMER_D_OUTPUTMODE_RESET_SET;
    initCompParam.compareValue = 21;
    Timer_D_initCompareMode(TIMER_D1_BASE, &initCompParam);

    Timer_D_initUpModeParam initUpParam = {0};
    initUpParam.clockSource = TIMER_D_CLOCKSOURCE_ACLK;
    initUpParam.clockSourceDivider = TIMER_D_CLOCKSOURCE_DIVIDER_1;
    initUpParam.clockingMode = TIMER_D_CLOCKINGMODE_EXTERNAL_CLOCK;
    initUpParam.timerPeriod = 82-1;
    initUpParam.timerInterruptEnable_TDIE = TIMER_D_TDIE_INTERRUPT_DISABLE;
    initUpParam.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_D_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initUpParam.timerClear = TIMER_D_DO_CLEAR;
    Timer_D_initUpMode(TIMER_D1_BASE, &initUpParam);

    Timer_D_startCounter(TIMER_D1_BASE,
    		TIMER_D_UP_MODE
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
    	TIMER_D_CAPTURECOMPARE_REGISTER_1);

    Timer_D_initCaptureModeParam initCapParam = {0};
    initCapParam.captureRegister = TIMER_D_CAPTURECOMPARE_REGISTER_1;
    initCapParam.captureMode = TIMER_D_CAPTUREMODE_RISING_AND_FALLING_EDGE;
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

      Period = REdge2 - REdge1;             // Calculate Period
      ON_Period = FEdge1-REdge1;             // On period
      DutyCycle = ((uint32_t)ON_Period*100/Period);
      if(DutyCycle!= 25)
      {
          GPIO_setOutputHighOnPin(
                           		  GPIO_PORT_P1,
                           		  GPIO_PIN0
                           		  );
      }
    }
}

// TD0_D1 Interrupt vector
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_D1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER0_D1_VECTOR)))
#endif
void TIMER0_D1_ISR (void)
{
    switch(__even_in_range(TD0IV,0x1E))
    {
      case  0x0: break;                     // Vector  0:  No interrupt
      case  0x2:                            // Vector  2:  TDCCR1 CCIFG
        if(!Count)
        {
             FEdge1 = // 1st falling edge
            		 Timer_D_getCaptureCompareCount(TIMER_D0_BASE,
            		           				TIMER_D_CAPTURECOMPARE_REGISTER_1);
             REdge1 = // 1st rising edge
            		 Timer_D_getCaptureCompareLatchCount(TIMER_D0_BASE,
            		   	          				TIMER_D_CAPTURECOMPARE_REGISTER_1);
             Count++;
        }
        else
        {
              REdge2 = // 2nd rising edge
            		  Timer_D_getCaptureCompareLatchCount(TIMER_D0_BASE,
            		              		   	          				TIMER_D_CAPTURECOMPARE_REGISTER_1);
              FEdge2 = // Dummy Read
            		  Timer_D_getCaptureCompareCount(TIMER_D0_BASE,
            		            				TIMER_D_CAPTURECOMPARE_REGISTER_1);
              Count = 0x0;
              __bic_SR_register_on_exit(LPM0_bits + GIE);  // Exit LPM0 on return to main
         }
         break;
      case 0x4: break;                      // Vector  4:  TDCCR2 CCIFG
      case 0x6: break;                      // Vector  6:  TDCCR3 CCIFG
      case 0x8: break;                      // Vector  8:  TDCCR4 CCIFG
      case 0xA: break;                      // Vector 10:  TDCCR5 CCIFG
      case 0xC: break;                      // Vector 12:  TDCCR5 CCIFG
      case 0xE: break;                      // Vector 14:  -
      case 0x10:break;                      // Vector 16:  TDIFG
      case 0x12:break;                      // Vector 18:  TDHINT TDHFLIFG
      case 0x14:break;                      // Vector 20:  TDHINT TDHFHIFG
      case 0x16:break;                      // Vector 22:  TDHINT TDHLKIFG
      case 0x18:break;                      // Vector 24:  TDHINT TDHUNLKIFG
      case 0x1A:break;                      // Vector 26:  -
      case 0x1C:break;                      // Vector 28:  -
      case 0x1E:break;                      // Vector 28:  -
      default: 	break;
    }
}


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
//!  The PMM is at default core level 0.
//!  The SVS high side (Vcc) is configured for Normal Performance mode.
//!  The SVS low side (Vcore) is configured for Normal Performance mode.
//!  Both SVS low side and SVS high side are disabled in LPM3
//!  Specs:
//!  Wake up from interrupt: 100us*
//!  Response to SVS event (propogation delay): 20us*
//!  Current consumption by SVS: 400nA* (SVSL = SVSH = 200nA)
//! //!Note: Refer to the device D/s for actual values
//!
//!  The timer outputs a PWM with a frequency ~800Hz. The LED at P1.0 is toggled
//!  on entry into the ISR. The time difference between PWM event and LED
//!  toggle = SVS wakeup time + interrupt latency + LED turn on = ~160+us.
//!
//!  NOTE: TERMINATE THE DEBUG SESSION BEFORE TESTING THIS EXAMPLE
//!  Refer section 2.2.8.1 for debug impact on SVS
//!
//!  ACLK = REFO = TACLK = ~32KHz, MCLK = SMCLK = default DCO ~1.045MHz
//!
//!           MSP430F54xA
//!         ---------------
//!     /|\|               |
//!      | |               |
//!      --|RST            |
//!        |          TA0.4|--> output from timer
//!        |           P1.0|--> LED
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - PMM peripheral
//! - GPIO Port peripheral
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - Timer0_A5.
//******************************************************************************

#include "driverlib.h"

#define DUTY_CYCLE1     32
#define TIMER_PERIOD    40

void main (void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    //Set DCO FLL reference = REFO
    UCS_initClockSignal(UCS_FLLREF,
        UCS_REFOCLK_SELECT,
        UCS_CLOCK_DIVIDER_1
        );

    //Set ACLK = REFO
    UCS_initClockSignal(UCS_ACLK,
        UCS_REFOCLK_SELECT,
        UCS_CLOCK_DIVIDER_1
        );

    //~ 32*32768 = 1048576
    UCS_initFLLSettle(1048,
        32
        );

    //Monitor low side is turned off
    PMM_disableSvmL();

    //Monitor high side is turned off
    PMM_disableSvmH();

    //SVS Low side is turned on
    PMM_enableSvsL();

    //Reset (POR) on SVS trigger
    PMM_enableSvsLReset();

    //Slow wake up from LPMs,// SVSL turns off in LPM3,power-save
    PMM_disableSvsLInLPMSlowWake();

    //SVSL turns off in LPM3,power-save
    PMM_enableSvsH();

    //SVS High side is turned on
    PMM_enableSvsHReset();

    //Normal Perf mode,
    PMM_disableSvsHInLPMNormPerf();
    //Wait until high side, low side settled
    while ( 0 == PMM_getInterruptStatus(
                PMM_SVSMLDLYIFG + PMM_SVSMHDLYIFG )
            ) ;

    //SVSH turns off in LPM3,power-save
    PMM_clearPMMIFGS();

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

    //P1.5 TA0.4 output 50% duty cycle
    //Set P1.0 to output direction
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN5
        );

    //Start timer in upMode sourced by ACLK
	Timer_A_clearTimerInterrupt(TIMER_A0_BASE);

    Timer_A_initUpModeParam initUpParam = {0};
    initUpParam.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
    initUpParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    initUpParam.timerPeriod = TIMER_PERIOD;
    initUpParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
    initUpParam.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    initUpParam.timerClear = TIMER_A_DO_CLEAR;
    initUpParam.startTimer = true;
    Timer_A_initUpMode(TIMER_A0_BASE, &initUpParam);

    //Generate PWM using capture
	Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,
		TIMER_A_CAPTURECOMPARE_REGISTER_4
		);

    Timer_A_initCompareModeParam initCompParam = {0};
    initCompParam.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4;
    initCompParam.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    initCompParam.compareOutputMode = TIMER_A_OUTPUTMODE_SET_RESET;
    initCompParam.compareValue = DUTY_CYCLE1;
    Timer_A_initCompareMode(TIMER_A0_BASE, &initCompParam);

    //Enter LPM3, enable interrupts
    __bis_SR_register(LPM3_bits + GIE);
    //For debugger
    __no_operation();
}

//******************************************************************************
//
//This is the Timer0_A5 interrupt vector service routine.
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER0_A1_VECTOR)))
#endif
void TIMER0_A1_ISR (void)
{
    switch (__even_in_range(TA0IV,14)){
        case  0: break;                          //No interrupt
        case  2: break;                          //CCR1 not used
        case  4: break;                          //CCR2 not used
        case  6: break;                          //reserved
        case  8: break;                          //reserved
        case 10: break;                          //reserved
        case 12: break;                          //reserved
        case 14:                                 //overflow
            //Toggle P1.0
            GPIO_toggleOutputOnPin(
            GPIO_PORT_P1,
            GPIO_PIN0
            );
            break;
        default: break;
    }
}


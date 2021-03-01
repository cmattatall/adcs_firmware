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
//! RTC_B in Calendar mode, Interruptions every 1s, 1m, and 5th day of
//!     week at 5:00pm
//!
//!  This program demonstrates the RTC mode by triggering an interrupt every
//!  second and minute. This code toggles P1.0 every second. This code
//!  recommends an external LFXT1 crystal for RTC accuracy.
//!  Note that if XT1 is not present the code loops in an infinite loop.
//!  ACLK = LFXT1 = 32768Hz, MCLK = SMCLK = default DCO = 32 x ACLK = 1048576Hz
//!
//!  Tested on: MSP430FR5739
//!             -----------------
//!        /|\ |              XIN|-
//!         |  |                 | 32kHz
//!         ---|RST          XOUT|-
//!            |                 |
//!            |            P1.0 |--> Toggles every second
//!            |                 |
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - RTC peripheral
//! - UCS peripheral
//! - GPIO Port peripheral
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - RTC_B_VECTOR
//!
//******************************************************************************

#include "driverlib.h"

volatile Calendar newTime;

void main (void)
{
    Calendar currentTime;

    WDT_A_hold(WDT_A_BASE);

    //Set P1.0 to output direction
    GPIO_setAsOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN0
        );

    /*
	* Select Port J
	* Set Pin 4, 5 to input Primary Module Function, LFXT.
	*/
	GPIO_setAsPeripheralModuleFunctionInputPin(
		GPIO_PORT_PJ,
		GPIO_PIN4 + GPIO_PIN5,
		GPIO_PRIMARY_MODULE_FUNCTION
	);


    //Initialize XT1
	CS_turnOnXT1(CS_XT1_DRIVE_0);

    //Setup Current Time for Calendar
    currentTime.Seconds    = 0x00;
    currentTime.Minutes    = 0x26;
    currentTime.Hours      = 0x13;
    currentTime.DayOfWeek  = 0x03;
    currentTime.DayOfMonth = 0x20;
    currentTime.Month      = 0x07;
    currentTime.Year       = 0x2011;

    //Initialize Calendar Mode of RTC
    /*
     * Base Address of the RTC_B
     * Pass in current time, intialized above
     * Use BCD as Calendar Register Format
     */
    RTC_B_initCalendar(RTC_B_BASE,
        &currentTime,
        RTC_B_FORMAT_BCD);

    //Setup Calendar Alarm for 5:00pm on the 5th day of the week.
    //Note: Does not specify day of the week.
    RTC_B_configureCalendarAlarmParam param = {0};
    param.minutesAlarm = 0x00;
    param.hoursAlarm = 0x17;
    param.dayOfWeekAlarm = RTC_B_ALARMCONDITION_OFF;
    param.dayOfMonthAlarm = 0x05;
    RTC_B_configureCalendarAlarm(RTC_B_BASE, &param);

    //Specify an interrupt to assert every minute
    RTC_B_setCalendarEvent(RTC_B_BASE,
        RTC_B_CALENDAREVENT_MINUTECHANGE);

    //Enable interrupt for RTC Ready Status, which asserts when the RTC
    //Calendar registers are ready to read.
    //Also, enable interrupts for the Calendar alarm and Calendar event.

	RTC_B_clearInterrupt(RTC_B_BASE,
        RTCRDYIE + RTCTEVIE + RTCAIE);

    RTC_B_enableInterrupt(RTC_B_BASE,
        RTCRDYIE + RTCTEVIE + RTCAIE);

    //Start RTC Clock
    RTC_B_startClock(RTC_B_BASE);

    //Enter LPM3 mode with interrupts enabled
    __bis_SR_register(LPM0_bits + GIE);
    __no_operation();
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=RTC_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(RTC_VECTOR)))
#endif
void RTC_B_ISR (void)
{
    switch (__even_in_range(RTCIV,16)){
        case 0: break;  //No interrupts
        case 2:         //RTCRDYIFG
            //Toggle P1.0 every second
            GPIO_toggleOutputOnPin(
            GPIO_PORT_P1,
            GPIO_PIN0);
            break;
        case 4:         //RTCEVIFG
            //Interrupts every minute
            __no_operation();

            //Read out New Time a Minute Later BREAKPOINT HERE
            newTime = RTC_B_getCalendarTime(RTC_B_BASE);
            break;
        case 6:         //RTCAIFG
            //Interrupts 5:00pm on 5th day of week
            __no_operation();
            break;
        case 8: break;  //RT0PSIFG
        case 10: break; //RT1PSIFG
        case 12: break; //Reserved
        case 14: break; //Reserved
        case 16: break; //Reserved
        default: break;
    }
}


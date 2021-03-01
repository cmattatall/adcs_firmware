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
/*******************************************************************************
 * MSP430i2xx Power Management Module - Voltage Monitor 1160mV
 *
 * Description: In this example, the device is setup to compare the VMONIN pin
 * to 1160mV. When the VMONIN pin drops below 1160mV the VMON interrupt fires
 * and the ISR is run. The voltage monitor interrupt flag is not clearable
 * by software and will only stop interrupting if VMON interrupt is disabled
 * or VMONIN goes above 1160mV. The LED is turned off when the voltage monitor
 * pin is below 1160mV and turns on when it is above. You can also use the
 * voltage monitor to monitor DVCC.
 *
 *
 *                MSP430i2041
 *             ------------------
 *         /|\|                  |
 *          | |              P1.4|-->LED
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |              P2.3|<--VMONIN
 *            |                  |
 *            |                  |
 *
 * Author: Zack Lalanne
 ******************************************************************************/

#include "driverlib.h"

int main(void) {

    WDT_hold(WDT_BASE);

    // Setup the LED
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN4);

    // Setup the voltage monitor to compare VMONIN to 1160mV
    PMM_setupVoltageMonitor(PMM_VMONIN_1160MV);

    // When started up, VMON requires a settling time of 0.5us
    // MCLK = ~16MHz; 1 cycle = ~0.0625us
    __delay_cycles(10);

    // Setup VMONIN Pin
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2,
                                               GPIO_PIN3,
                                               GPIO_TERNARY_MODULE_FUNCTION);

    // Enable the voltage monitor interrupt
    PMM_enableInterrupt(PMM_VMON_INTERRUPT);
    __bis_SR_register(GIE);
#if defined(__GNUC__)
    // Not necessary in Rev. D silicon, but appeases a warning for GCC
    __asm(" nop");
#endif

    while(1) {
        // If we are not in ISR then VMONIN > 1160mV
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN4);
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=VMON_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(VMON_VECTOR)))
#endif
void VMON_ISR(void) {
    // VMONIN < 1160mV, LED Off
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN4);
}

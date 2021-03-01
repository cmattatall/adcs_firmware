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
 * MSP430i2xx EUSCI_B I2C - Master transfer multiple bytes
 *
 * Description: In this example, the device is configured as an I2C master. It
 * starts an I2C transaction in active mode but finishes communicating the data
 * while still in LPM0. After the last byte has been sent it exits LPM0 to
 * start the transaction over again. This example can be modified to send data
 * of an arbitrary length by modifying the main loop. Start the corresponding
 * slave code before running this master code.
 *
 *
 *                                /|\  /|\
 *                MSP430i2041     10k  10k      MSP430i2041
 *                   slave         |    |         master
 *             -----------------   |    |   -----------------
 *            |     P1.6/UCB0SCL|<-|----+->|P1.6/UCB0SCL     |
 *            |                 |  |       |                 |
 *            |                 |  |       |                 |
 *            |     P1.7/UCB0SDA|<-+------>|P1.7/UCB0SDA     |
 *            |                 |          |                 |
 *
 * Author: Zack Lalanne
 ******************************************************************************/

#include "driverlib.h"

#define SLAVE_ADDRESS 0x48

static uint8_t TXData = 1;
static uint8_t TXByteCtr;

int main(void) {

    EUSCI_B_I2C_initMasterParam i2cConfig = {
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        4096000,                                // SMCLK = 4.096MHz
        EUSCI_B_I2C_SET_DATA_RATE_400KBPS,      // Desired I2C Clock of 400kHz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
    };

    WDT_hold(WDT_BASE);

    // Setting the DCO to use the internal resistor. DCO will be at 16.384MHz
    CS_setupDCO(CS_INTERNAL_RESISTOR);

    // Setting SMCLK = DC0 / 4 = 4.096 MHz
    CS_initClockSignal(CS_SMCLK, CS_CLOCK_DIVIDER_4);

    // Setting P1.6 and P1.7 as I2C pins
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                                               GPIO_PIN6 | GPIO_PIN7,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    // Setting up I2C communication at 400kHz using SMCLK
    EUSCI_B_I2C_initMaster(EUSCI_B0_BASE, &i2cConfig);

    // Settings slave address
    EUSCI_B_I2C_setSlaveAddress(EUSCI_B0_BASE, SLAVE_ADDRESS);

    // Enable the module for operation
    EUSCI_B_I2C_enable(EUSCI_B0_BASE);

    // Enable needed I2C interrupts
    EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0 |
                                                  EUSCI_B_I2C_NAK_INTERRUPT);
    EUSCI_B_I2C_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0 |
                                               EUSCI_B_I2C_NAK_INTERRUPT);

    while(1) {

        TXByteCtr = 4;
        TXData = 0;

        // Make sure last transaction is done sending
        while(EUSCI_B_I2C_masterIsStopSent(EUSCI_B0_BASE) == EUSCI_B_I2C_SENDING_STOP);

        TXByteCtr--;
        EUSCI_B_I2C_masterSendMultiByteStart(EUSCI_B0_BASE, TXData++);

        // Go to sleep and wait for LPM exit
        __bis_SR_register(LPM0_bits | GIE);
    }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_B0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_B0_VECTOR)))
#endif
void USCIB0_ISR(void) {
    switch(__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG)) {
        case USCI_NONE: break;
        case USCI_I2C_UCALIFG: break;
        case USCI_I2C_UCNACKIFG:
            EUSCI_B_I2C_masterSendStart(EUSCI_B0_BASE);
            break;
        case USCI_I2C_UCSTTIFG: break;
        case USCI_I2C_UCSTPIFG: break;
        case USCI_I2C_UCRXIFG3: break;
        case USCI_I2C_UCTXIFG3: break;
        case USCI_I2C_UCRXIFG2: break;
        case USCI_I2C_UCTXIFG2: break;
        case USCI_I2C_UCRXIFG1: break;
        case USCI_I2C_UCTXIFG1: break;
        case USCI_I2C_UCRXIFG0: break;
        case USCI_I2C_UCTXIFG0:
            if(TXByteCtr) {
                EUSCI_B_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, TXData++);
                TXByteCtr--;
            } else {
                EUSCI_B_I2C_masterSendMultiByteStop(EUSCI_B0_BASE);

                // Exit LPM to start next transaction
                __bic_SR_register_on_exit(LPM0_bits);
            }
            break;
        case USCI_I2C_UCBCNTIFG: break;
        case USCI_I2C_UCCLTOIFG: break;
        case USCI_I2C_UCBIT9IFG: break;
        default: break;
    }
}

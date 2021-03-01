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
 * MSP430i2xx EUSCI_B I2C - Slave transfer multiple bytes
 *
 * Description: In this example, the device is configured as an I2C slave.
 * When the slave has data available it pulls the IRQ line (P1.0) down to
 * tell the master data is ready. The master interrupts on the GPIO transition
 * and starts an I2C read. The first byte of the slave's packet is the length
 * of the packet, including the length byte. The master then continues to
 * read bytes until all data from the packet has been consumed. All data
 * processing is done in the interrupt service routines and the device stays
 * in LPM0. Run the corresponding master example before starting this example.
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
 *            |             P1.0|--------->|P1.0             |
 *            |                 |          |                 |
 *
 * Author: Zack Lalanne
 ******************************************************************************/

#include "driverlib.h"

#define SLAVE_ADDRESS       0x48
#define NUM_OF_TX_BYTES     4

const uint8_t TXData[NUM_OF_TX_BYTES] = {NUM_OF_TX_BYTES,
                                         0xBE,
                                         0xEF,
                                         0xA5};

static volatile uint16_t TXDataIndex = 0;

int main(void) {

    EUSCI_B_I2C_initSlaveParam i2cConfig = {
        SLAVE_ADDRESS,                              // Slave Address
        EUSCI_B_I2C_OWN_ADDRESS_OFFSET0,
        EUSCI_B_I2C_OWN_ADDRESS_ENABLE
    };

    WDT_hold(WDT_BASE);

    // Setting the DCO to use the internal resistor. DCO will be at 16.384MHz
    CS_setupDCO(CS_INTERNAL_RESISTOR);

    // Setting P1.6 and P1.7 as I2C pins
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                                               GPIO_PIN6 | GPIO_PIN7,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    // Using P1.0 as slave interrupt pin
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    // Setting up I2C communication as slave
    EUSCI_B_I2C_initSlave(EUSCI_B0_BASE, &i2cConfig);

    // Set master in transmit mode
    EUSCI_B_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    // Enable the module for operation
    EUSCI_B_I2C_enable(EUSCI_B0_BASE);

    while(1) {

        // Reset the TX Buffer
        EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE,
                                       EUSCI_B_I2C_TRANSMIT_INTERRUPT0);

        // Interrupt when TX Buffer is empty or stop from master
        EUSCI_B_I2C_enableInterrupt(EUSCI_B0_BASE,
                                    EUSCI_B_I2C_TRANSMIT_INTERRUPT0 |
                                    EUSCI_B_I2C_STOP_INTERRUPT);

        // Tell master that data is ready
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

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
        case USCI_I2C_UCNACKIFG: break;
        case USCI_I2C_UCSTTIFG: break;
        case USCI_I2C_UCSTPIFG:

            // No more data, set IRQ high
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
            TXDataIndex = 0;

            // Exit LPM to start next transaction
            __bic_SR_register_on_exit(LPM0_bits | GIE);
            break;
        case USCI_I2C_UCRXIFG3: break;
        case USCI_I2C_UCTXIFG3: break;
        case USCI_I2C_UCRXIFG2: break;
        case USCI_I2C_UCTXIFG2: break;
        case USCI_I2C_UCRXIFG1: break;
        case USCI_I2C_UCTXIFG1: break;
        case USCI_I2C_UCRXIFG0: break;
        case USCI_I2C_UCTXIFG0:
            EUSCI_B_I2C_slavePutData(EUSCI_B0_BASE, TXData[TXDataIndex++]);

            // Disable TX if all data is sent
            if(TXDataIndex == NUM_OF_TX_BYTES) {
                EUSCI_B_I2C_disableInterrupt(EUSCI_B0_BASE,
                                             EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
            }
            break;
        case USCI_I2C_UCBCNTIFG: break;
        case USCI_I2C_UCCLTOIFG: break;
        case USCI_I2C_UCBIT9IFG: break;
        default: break;
    }
}

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
#include "driverlib.h"

//******************************************************************************
//  Demo - EUSCI_B0 I2C Master TX bytes to Multiple Slaves
//
//  Description: This demo connects two MSP430's via the I2C bus.
//  The master transmits to 4 different I2C slave addresses 0x0A,0x0B,0x0C&0x0D.
//  Each slave address has a specific related data in the array TXData[].
//  At the end of four I2C transactions the slave address rolls over and begins
//  again at 0x0A.
//  ACLK = n/a, MCLK = SMCLK = BRCLK = default DCO = ~1.045MHz
//
//                                /|\  /|\
//                MSP430FR5969    10k  10k     MSP430FR5969
//                   slave         |    |         master
//             -----------------   |    |   -----------------
//           -|XIN  P1.6/UCB0SDA|<-|----+->|P1.6/UCB0SDA  XIN|-
//            |                 |  |       |                 |
//           -|XOUT             |  |       |             XOUT|-
//            |     P1.7/UCB0SCL|<-+------>|P1.7/UCB0SCL     |
//            |                 |          |                 |
//
//******************************************************************************


uint8_t TXData[]= {0xA1,0xB1,0xC1,0xD1};// Pointer to TX data
uint8_t SlaveAddress[]= {0x0A,0x0B,0x0C,0x0D};
uint8_t TXByteCtr;
uint8_t SlaveFlag = 0;

void main(void)
{
    WDT_A_hold(WDT_A_BASE);

      //Set DCO frequency to 1MHz
    CS_setDCOFreq(CS_DCORSEL_0,CS_DCOFSEL_0);
    //Set ACLK = VLO with frequency divider of 1
    CS_initClockSignal(CS_ACLK,CS_VLOCLK_SELECT,CS_CLOCK_DIVIDER_1);
    //Set SMCLK = DCO with frequency divider of 1
    CS_initClockSignal(CS_SMCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);
    //Set MCLK = DCO with frequency divider of 1
    CS_initClockSignal(CS_MCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);

    // Configure Pins for I2C
    //Set P1.6 and P1.7 as Secondary Module Function Input.
    /*

    * Select Port 1
    * Set Pin 6, 7 to input Secondary Module Function, (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
    */
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P1,
        GPIO_PIN6 + GPIO_PIN7,
        GPIO_SECONDARY_MODULE_FUNCTION
    );

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    EUSCI_B_I2C_initMasterParam param = {0};
    param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = CS_getSMCLK();
    param.dataRate = EUSCI_B_I2C_SET_DATA_RATE_400KBPS;
    param.byteCounterThreshold = 0;
    param.autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP;
    EUSCI_B_I2C_initMaster(EUSCI_B0_BASE, &param);


    //Set Master in receive mode
    EUSCI_B_I2C_setMode(EUSCI_B0_BASE,
                    EUSCI_B_I2C_TRANSMIT_MODE
                    );
    //Enable I2C Module to start operations
    EUSCI_B_I2C_enable(EUSCI_B0_BASE);

    EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE,
                EUSCI_B_I2C_TRANSMIT_INTERRUPT0 +
                    EUSCI_B_I2C_NAK_INTERRUPT
                );

    //Enable master Receive interrupt
    EUSCI_B_I2C_enableInterrupt(EUSCI_B0_BASE,
                    EUSCI_B_I2C_TRANSMIT_INTERRUPT0 +
                    EUSCI_B_I2C_NAK_INTERRUPT
                  );

    SlaveFlag = 0;
    while(1)
    {
        //Specify slave address
        EUSCI_B_I2C_setSlaveAddress(EUSCI_B0_BASE,
                SlaveAddress[SlaveFlag]
                );


        TXByteCtr = 1;                          // Load TX byte counter

        while (EUSCI_B_I2C_SENDING_STOP == EUSCI_B_I2C_masterIsStopSent
                (EUSCI_B0_BASE));


        EUSCI_B_I2C_masterSendStart(EUSCI_B0_BASE);

        __bis_SR_register(CPUOFF + GIE);        // Enter LPM0 w/ interrupts
        // Remain in LPM0 until all data is TX'd

        // Change Slave address
        SlaveFlag++;
        if(SlaveFlag > 3)                        // Roll over slave address
        {
            SlaveFlag = 0;
            __delay_cycles(1000);               // Delay between transmissions
        }
    }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_B0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_B0_VECTOR)))
#endif
void USCIB0_ISR(void)
{
    switch(__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG))
  {
        case USCI_NONE:             // No interrupts break;
            break;
        case USCI_I2C_UCALIFG:      // Arbitration lost
            break;
        case USCI_I2C_UCNACKIFG:    // NAK received (master only)
            // Resend START if NAK'd
            EUSCI_B_I2C_masterSendStart(EUSCI_B0_BASE);
            break;
        case USCI_I2C_UCSTTIFG:     // START condition detected with own address (slave mode only)
            break;
        case USCI_I2C_UCSTPIFG:     // STOP condition detected (master & slave mode)
            break;
        case USCI_I2C_UCRXIFG3:     // RXIFG3
            break;
        case USCI_I2C_UCTXIFG3:     // TXIFG3
            break;
        case USCI_I2C_UCRXIFG2:     // RXIFG2
            break;
        case USCI_I2C_UCTXIFG2:     // TXIFG2
            break;
        case USCI_I2C_UCRXIFG1:     // RXIFG1
            break;
        case USCI_I2C_UCTXIFG1:     // TXIFG1
            break;
        case USCI_I2C_UCRXIFG0:     // RXIFG0
            break;
        case USCI_I2C_UCTXIFG0:     // TXIFG0
            // Check TX byte counter
            if (TXByteCtr)
            {
                EUSCI_B_I2C_masterSendMultiByteNext(EUSCI_B0_BASE,
                    TXData[SlaveFlag]);
                // Decrement TX byte counter
                TXByteCtr--;
            }
            else
            {
                EUSCI_B_I2C_masterSendMultiByteStop(EUSCI_B0_BASE);
                // Exit LPM0
                __bic_SR_register_on_exit(CPUOFF);
            }
            break;
        case USCI_I2C_UCBCNTIFG:    // Byte count limit reached (UCBxTBCNT)
            break;
        case USCI_I2C_UCCLTOIFG:    // Clock low timeout - clock held low too long
            break;
        case USCI_I2C_UCBIT9IFG:    // Generated on 9th bit of a transmit (for debugging)
            break;
        default:
            break;
  }
}


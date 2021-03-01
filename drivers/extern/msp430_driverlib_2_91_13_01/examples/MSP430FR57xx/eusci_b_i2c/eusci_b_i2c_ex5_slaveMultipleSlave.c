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
//  MSP430FR57xx Demo - USCI_B0 I2C 4 Hardware I2C slaves
//
//  Description: This demo connects two MSP430's via the I2C bus.
//  This code configures the MSP430 USCI to be addressed as 4 independent I2C
//  slaves. Each slave has its owm interrupt flag and data variable to store
//  incoming data.
//
//	 Tested on MSP430FR5739, MSP430FR5969
//                                /|\  /|\
//                                10k  10k
//                   slave         |    |         master
//             -----------------   |    |   -----------------
//           -|XIN  P1.6/UCB0SDA|<-|----+->|P1.6/UCB0SDA  XIN|-
//            |                 |  |       |                 |
//           -|XOUT             |  |       |             XOUT|-
//            |     P1.7/UCB0SCL|<-+------>|P1.7/UCB0SCL     |
//            |                 |          |                 |
//            |                 |          |                 |
//
//******************************************************************************
uint8_t RXData0=0;
uint8_t RXData1=0;
uint8_t RXData2=0;
uint8_t RXData3=0;

void main(void)
{
	WDT_A_hold(WDT_A_BASE);

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

    // eUSCI configuration
    //SLAVE0 own address is 0x0A+ enable
    EUSCI_B_I2C_initSlaveParam initSlave0Param = {0};
    initSlave0Param.slaveAddress = 0x0A;
    initSlave0Param.slaveAddressOffset = EUSCI_B_I2C_OWN_ADDRESS_OFFSET0;
    initSlave0Param.slaveOwnAddressEnable = EUSCI_B_I2C_OWN_ADDRESS_ENABLE;
    EUSCI_B_I2C_initSlave(EUSCI_B0_BASE, &initSlave0Param);

    //SLAVE1 own address is 0x0B+ enable
    EUSCI_B_I2C_initSlaveParam initSlave1Param = {0};
    initSlave1Param.slaveAddress = 0x0B;
    initSlave1Param.slaveAddressOffset = EUSCI_B_I2C_OWN_ADDRESS_OFFSET1;
    initSlave1Param.slaveOwnAddressEnable = EUSCI_B_I2C_OWN_ADDRESS_ENABLE;
    EUSCI_B_I2C_initSlave(EUSCI_B0_BASE, &initSlave1Param);

    //SLAVE2 own address is 0x0C+ enable
    EUSCI_B_I2C_initSlaveParam initSlave2Param = {0};
    initSlave2Param.slaveAddress = 0x0C;
    initSlave2Param.slaveAddressOffset = EUSCI_B_I2C_OWN_ADDRESS_OFFSET2;
    initSlave2Param.slaveOwnAddressEnable = EUSCI_B_I2C_OWN_ADDRESS_ENABLE;
    EUSCI_B_I2C_initSlave(EUSCI_B0_BASE, &initSlave2Param);

    //SLAVE3 own address is 0x0D+ enable
    EUSCI_B_I2C_initSlaveParam initSlave3Param = {0};
    initSlave3Param.slaveAddress = 0x0D;
    initSlave3Param.slaveAddressOffset = EUSCI_B_I2C_OWN_ADDRESS_OFFSET3;
    initSlave3Param.slaveOwnAddressEnable = EUSCI_B_I2C_OWN_ADDRESS_ENABLE;
    EUSCI_B_I2C_initSlave(EUSCI_B0_BASE, &initSlave3Param);

    EUSCI_B_I2C_enable(EUSCI_B0_BASE);

	EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE,
					EUSCI_B_I2C_RECEIVE_INTERRUPT0 +
    				EUSCI_B_I2C_RECEIVE_INTERRUPT1 +
    				EUSCI_B_I2C_RECEIVE_INTERRUPT2 +
    				EUSCI_B_I2C_RECEIVE_INTERRUPT3
           		);

    //receive interrupt enable
    EUSCI_B_I2C_enableInterrupt(EUSCI_B0_BASE,
    				EUSCI_B_I2C_RECEIVE_INTERRUPT0 +
    				EUSCI_B_I2C_RECEIVE_INTERRUPT1 +
    				EUSCI_B_I2C_RECEIVE_INTERRUPT2 +
    				EUSCI_B_I2C_RECEIVE_INTERRUPT3
    				);



    __bis_SR_register(CPUOFF + GIE);        // Enter LPM0 w/ interrupts
    __no_operation();
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_B0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_B0_VECTOR)))
#endif
void USCIB0_ISR(void)
{
   switch(__even_in_range(UCB0IV,0x1E))
    {
      case 0x00: break;                     // Vector 0: No interrupts break;
      case 0x02: break;                     // Vector 2: ALIFG break;
      case 0x04: break;                     // Vector 4: NACKIFG break;
      case 0x06: break;                     // Vector 6: STTIFG break;
      case 0x08: break;                     // Vector 8: STPIFG break;
      case 0x0a:                            // SLAVE3
        RXData3 = EUSCI_B_I2C_slaveGetData(EUSCI_B0_BASE);
        break;                              // Vector 10: RXIFG3 break;
      case 0x0c: break;                     // Vector 14: TXIFG3 break;
      case 0x0e:                            // SLAVE2
        RXData2 = EUSCI_B_I2C_slaveGetData(EUSCI_B0_BASE);
        break;                              // Vector 16: RXIFG2 break;
      case 0x10: break;                     // Vector 18: TXIFG2 break;
      case 0x12:                            // SLAVE1
        RXData1 = EUSCI_B_I2C_slaveGetData(EUSCI_B0_BASE);
        break;                              // Vector 20: RXIFG1 break;
      case 0x14: break;                     // Vector 22: TXIFG1 break;
      case 0x16:                            // SLAVE0
        RXData0 = EUSCI_B_I2C_slaveGetData(EUSCI_B0_BASE);
        // Get RX data
        break;                              // Vector 24: RXIFG0 break;
      case 0x18: break;                     // Vector 26: TXIFG0 break;
      case 0x1a: break;                     // Vector 28: BCNTIFG break;
      case 0x1c: break;                     // Vector 30: clock low timeout break;
      case 0x1e: break;                     // Vector 32: 9th bit break;
      default: break;
    }

}



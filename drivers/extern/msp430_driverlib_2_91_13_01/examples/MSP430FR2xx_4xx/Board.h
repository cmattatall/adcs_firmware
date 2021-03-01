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
/**
 * ===
 * Board.h
 * ===
 * This file is meant to provide generic GPIO selection definitions that allows
 * DriverLib to be used portably across different LaunchPads. The same module
 * pin might not be populated on the same physical port and pin on different
 * LaunchPads.
 *
 * This file is intended to be modified and updated by individual users based
 * on application need. It will be initially populated according to common
 * peripherals available on a LaunchPad board, but by all means not complete.
 *
 * GPIO_PRIMARY_MODULE_FUNCTION indicates Port SEL bits are 01
 * GPIO_SECONDARY_MODULE_FUNCTION indicates Port SEL bits are 10
 * GPIO_TERNARY_MODULE_FUNCTION indicates Port SEL bits are 11
 * If the Port SEL bits are 00, the FUNCTION bits should be ignored
 * Please consult device datasheet for more info, under "Input/Output Diagrams"
 *
 **/

#ifndef __BOARD_H__
#define __BOARD_H__

#ifdef __MSP430FR4133__

#define GPIO_PORT_LED1          GPIO_PORT_P1
#define GPIO_PIN_LED1           GPIO_PIN0
#define GPIO_PORT_LED2          GPIO_PORT_P4
#define GPIO_PIN_LED2           GPIO_PIN0
#define GPIO_PORT_S1            GPIO_PORT_P1
#define GPIO_PIN_S1             GPIO_PIN2

#define GPIO_PORT_ADC7          GPIO_PORT_P1
#define GPIO_PIN_ADC7           GPIO_PIN7
#define GPIO_FUNCTION_ADC7      GPIO_PRIMARY_MODULE_FUNCTION

#define GPIO_PORT_MCLK          GPIO_PORT_P1
#define GPIO_PIN_MCLK           GPIO_PIN4
#define GPIO_FUNCTION_MCLK      GPIO_PRIMARY_MODULE_FUNCTION
#define GPIO_PORT_SMCLK         GPIO_PORT_P8
#define GPIO_PIN_SMCLK          GPIO_PIN0
#define GPIO_FUNCTION_SMCLK     GPIO_PRIMARY_MODULE_FUNCTION
#define GPIO_PORT_ACLK          GPIO_PORT_P8
#define GPIO_PIN_ACLK           GPIO_PIN1
#define GPIO_FUNCTION_ACLK      GPIO_PRIMARY_MODULE_FUNCTION

#define GPIO_PORT_UCA0TXD       GPIO_PORT_P1
#define GPIO_PIN_UCA0TXD        GPIO_PIN0
#define GPIO_FUNCTION_UCA0TXD   GPIO_PRIMARY_MODULE_FUNCTION
#define GPIO_PORT_UCA0RXD       GPIO_PORT_P1
#define GPIO_PIN_UCA0RXD        GPIO_PIN1
#define GPIO_FUNCTION_UCA0RXD   GPIO_PRIMARY_MODULE_FUNCTION

#define GPIO_PORT_UCB0SCL       GPIO_PORT_P5
#define GPIO_PIN_UCB0SCL        GPIO_PIN3
#define GPIO_FUNCTION_UCB0SCL   GPIO_PRIMARY_MODULE_FUNCTION
#define GPIO_PORT_UCB0SDA       GPIO_PORT_P5
#define GPIO_PIN_UCB0SDA        GPIO_PIN2
#define GPIO_FUNCTION_UCB0SDA   GPIO_PRIMARY_MODULE_FUNCTION

#endif // __MSP430FR4133__

#ifdef __MSP430FR2311__

#define GPIO_PORT_LED1          GPIO_PORT_P1
#define GPIO_PIN_LED1           GPIO_PIN0
#define GPIO_PORT_LED2          GPIO_PORT_P2
#define GPIO_PIN_LED2           GPIO_PIN0
#define GPIO_PORT_S1            GPIO_PORT_P1
#define GPIO_PIN_S1             GPIO_PIN1

#define GPIO_PORT_ADC7          GPIO_PORT_P1
#define GPIO_PIN_ADC7           GPIO_PIN7
#define GPIO_FUNCTION_ADC7      GPIO_PRIMARY_MODULE_FUNCTION

#define GPIO_PORT_MCLK          GPIO_PORT_P2
#define GPIO_PIN_MCLK           GPIO_PIN6
#define GPIO_FUNCTION_MCLK      GPIO_PRIMARY_MODULE_FUNCTION
#define GPIO_PORT_SMCLK         GPIO_PORT_P1
#define GPIO_PIN_SMCLK          GPIO_PIN0
#define GPIO_FUNCTION_SMCLK     GPIO_SECONDARY_MODULE_FUNCTION
#define GPIO_PORT_ACLK          GPIO_PORT_P1
#define GPIO_PIN_ACLK           GPIO_PIN1
#define GPIO_FUNCTION_ACLK      GPIO_SECONDARY_MODULE_FUNCTION

#define GPIO_PORT_UCA0TXD       GPIO_PORT_P1
#define GPIO_PIN_UCA0TXD        GPIO_PIN7
#define GPIO_FUNCTION_UCA0TXD   GPIO_PRIMARY_MODULE_FUNCTION
#define GPIO_PORT_UCA0RXD       GPIO_PORT_P1
#define GPIO_PIN_UCA0RXD        GPIO_PIN6
#define GPIO_FUNCTION_UCA0RXD   GPIO_PRIMARY_MODULE_FUNCTION

#define GPIO_PORT_UCB0SCL       GPIO_PORT_P1
#define GPIO_PIN_UCB0SCL        GPIO_PIN3
#define GPIO_FUNCTION_UCB0SCL   GPIO_PRIMARY_MODULE_FUNCTION
#define GPIO_PORT_UCB0SDA       GPIO_PORT_P1
#define GPIO_PIN_UCB0SDA        GPIO_PIN2
#define GPIO_FUNCTION_UCB0SDA   GPIO_PRIMARY_MODULE_FUNCTION

#endif // __MSP430FR2311__

#ifdef __MSP430FR2433__

#define GPIO_PORT_LED1          GPIO_PORT_P1
#define GPIO_PIN_LED1           GPIO_PIN0
#define GPIO_PORT_LED2          GPIO_PORT_P1
#define GPIO_PIN_LED2           GPIO_PIN1
#define GPIO_PORT_S1            GPIO_PORT_P2
#define GPIO_PIN_S1             GPIO_PIN3
#define GPIO_PORT_S2            GPIO_PORT_P2
#define GPIO_PIN_S2             GPIO_PIN7

#define GPIO_PORT_ADC7          GPIO_PORT_P1
#define GPIO_PIN_ADC7           GPIO_PIN7
#define GPIO_FUNCTION_ADC7      GPIO_PRIMARY_MODULE_FUNCTION

#define GPIO_PORT_MCLK          GPIO_PORT_P1
#define GPIO_PIN_MCLK           GPIO_PIN3
#define GPIO_FUNCTION_MCLK      GPIO_SECONDARY_MODULE_FUNCTION
#define GPIO_PORT_SMCLK         GPIO_PORT_P1
#define GPIO_PIN_SMCLK          GPIO_PIN7
#define GPIO_FUNCTION_SMCLK     GPIO_SECONDARY_MODULE_FUNCTION
#define GPIO_PORT_ACLK          GPIO_PORT_P2
#define GPIO_PIN_ACLK           GPIO_PIN2
#define GPIO_FUNCTION_ACLK      GPIO_SECONDARY_MODULE_FUNCTION

#define GPIO_PORT_UCA0TXD       GPIO_PORT_P1
#define GPIO_PIN_UCA0TXD        GPIO_PIN4
#define GPIO_FUNCTION_UCA0TXD   GPIO_PRIMARY_MODULE_FUNCTION
#define GPIO_PORT_UCA0RXD       GPIO_PORT_P1
#define GPIO_PIN_UCA0RXD        GPIO_PIN5
#define GPIO_FUNCTION_UCA0RXD   GPIO_PRIMARY_MODULE_FUNCTION

#define GPIO_PORT_UCB0SCL       GPIO_PORT_P1
#define GPIO_PIN_UCB0SCL        GPIO_PIN3
#define GPIO_FUNCTION_UCB0SCL   GPIO_PRIMARY_MODULE_FUNCTION
#define GPIO_PORT_UCB0SDA       GPIO_PORT_P1
#define GPIO_PIN_UCB0SDA        GPIO_PIN2
#define GPIO_FUNCTION_UCB0SDA   GPIO_PRIMARY_MODULE_FUNCTION

#endif // __MSP430FR2433__

#ifdef __MSP430FR2355__

#define GPIO_PORT_LED1          GPIO_PORT_P1
#define GPIO_PIN_LED1           GPIO_PIN0
#define GPIO_PORT_LED2          GPIO_PORT_P6
#define GPIO_PIN_LED2           GPIO_PIN6
#define GPIO_PORT_S1            GPIO_PORT_P4
#define GPIO_PIN_S1             GPIO_PIN1
#define GPIO_PORT_S2            GPIO_PORT_P2
#define GPIO_PIN_S2             GPIO_PIN3

#define GPIO_PORT_ADC7          GPIO_PORT_P1
#define GPIO_PIN_ADC7           GPIO_PIN7
#define GPIO_FUNCTION_ADC7      GPIO_TERNARY_MODULE_FUNCTION

#define GPIO_PORT_XIN           GPIO_PORT_P2
#define GPIO_PIN_XIN            GPIO_PIN_P7
#define GPIO_FUNCTION_XIN       GPIO_SECONDARY_MODULE_FUNCTION
#define GPIO_PORT_XOUT          GPIO_PORT_P2
#define GPIO_PIN_XOUT           GPIO_PIN_P6
#define GPIO_FUNCTION_XOUT      GPIO_SECONDARY_MODULE_FUNCTION

#define GPIO_PORT_MCLK          GPIO_PORT_P3
#define GPIO_PIN_MCLK           GPIO_PIN0
#define GPIO_FUNCTION_MCLK      GPIO_PRIMARY_MODULE_FUNCTION
#define GPIO_PORT_SMCLK         GPIO_PORT_P3
#define GPIO_PIN_SMCLK          GPIO_PIN4
#define GPIO_FUNCTION_SMCLK     GPIO_PRIMARY_MODULE_FUNCTION
#define GPIO_PORT_ACLK          GPIO_PORT_P1
#define GPIO_PIN_ACLK           GPIO_PIN1
#define GPIO_FUNCTION_ACLK      GPIO_SECONDARY_MODULE_FUNCTION

#define GPIO_PORT_UCA0TXD       GPIO_PORT_P1
#define GPIO_PIN_UCA0TXD        GPIO_PIN7
#define GPIO_FUNCTION_UCA0TXD   GPIO_PRIMARY_MODULE_FUNCTION
#define GPIO_PORT_UCA0RXD       GPIO_PORT_P1
#define GPIO_PIN_UCA0RXD        GPIO_PIN6
#define GPIO_FUNCTION_UCA0RXD   GPIO_PRIMARY_MODULE_FUNCTION

#define GPIO_PORT_UCB0SCL       GPIO_PORT_P1
#define GPIO_PIN_UCB0SCL        GPIO_PIN3
#define GPIO_FUNCTION_UCB0SCL   GPIO_PRIMARY_MODULE_FUNCTION
#define GPIO_PORT_UCB0SDA       GPIO_PORT_P1
#define GPIO_PIN_UCB0SDA        GPIO_PIN2
#define GPIO_FUNCTION_UCB0SDA   GPIO_PRIMARY_MODULE_FUNCTION

#define GPIO_PORT_SACOA0O       GPIO_PORT_P1
#define GPIO_PIN_SACOA0O        GPIO_PIN1
#define GPIO_FUNCTION_SACOA0O   GPIO_TERNARY_MODULE_FUNCTION
#define GPIO_PORT_SACOA0N       GPIO_PORT_P1
#define GPIO_PIN_SACOA0N        GPIO_PIN2
#define GPIO_FUNCTION_SACOA0N   GPIO_TERNARY_MODULE_FUNCTION
#define GPIO_PORT_SACOA0P       GPIO_PORT_P1
#define GPIO_PIN_SACOA0P        GPIO_PIN3
#define GPIO_FUNCTION_SACOA0P   GPIO_TERNARY_MODULE_FUNCTION

#define GPIO_PORT_SACOA2O       GPIO_PORT_P3
#define GPIO_PIN_SACOA2O        GPIO_PIN1
#define GPIO_FUNCTION_SACOA2O   GPIO_TERNARY_MODULE_FUNCTION
#define GPIO_PORT_SACOA2N       GPIO_PORT_P3
#define GPIO_PIN_SACOA2N        GPIO_PIN2
#define GPIO_FUNCTION_SACOA2N   GPIO_TERNARY_MODULE_FUNCTION
#define GPIO_PORT_SACOA2P       GPIO_PORT_P3
#define GPIO_PIN_SACOA2P        GPIO_PIN3
#define GPIO_FUNCTION_SACOA2P   GPIO_TERNARY_MODULE_FUNCTION

#define GPIO_PORT_COMP0O        GPIO_PORT_P2
#define GPIO_PIN_COMP0O         GPIO_PIN0
#define GPIO_FUNCTION_COMP0O    GPIO_SECONDARY_MODULE_FUNCTION

#endif // __MSP430FR2355__

#endif // __BOARD_H__


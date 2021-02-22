/**
 * @file i2c.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to implement an i2c driver using UCB0 for msp430f5529
 * @version 0.1
 * @date 2021-02-22
 * 
 * @copyright Copyright (c) 2021 Carl Mattatall
 * 
 */
#if !defined(TARGET_MCU)
#error DRIVER COMPILATION SHOULD ONLY OCCUR ON CROSSCOMPILED TARGETS
#endif /* !defined(TARGET_MCU) */

#include <msp430.h>

#include "i2c.h"


static void I2C0_PHY_init(void);


void I2C0_init(void)
{

    UCB0CTL1 |= UCSWRST; /* unlock peripheral to modify config */


    UCB0CTL1 &= ~UCSWRST;


}



static void I2C0_PHY_init(void)
{


/** @todo IMPLEMENT */

}

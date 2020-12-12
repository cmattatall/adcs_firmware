/**
 * @file mcu.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief
 * @version 0.1
 * @date 2020-12-12
 *
 * @copyright Copyright (c) 2020
 *
 * @note
 * @todo
 */

#if defined(TARGET_MCU)
#include <msp430.h>
#else
#include <stdio.h>
#endif /* #if defined(TARGET_MCU) */

#include "platform.h"


void enable_interrupts(void)
{
#if defined(TARGET_MCU)
    __bis_SR_register(LPM0_bits + GIE); /* Enter LPM0, interrupts enabled */
#else
    log_trace("enabled interrupts\n");
#endif /* #if defined(TARGET_MCU) */
}
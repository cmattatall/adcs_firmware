/**
 * @file watchdog.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to wrap dependencies for watchdog timer on msp430f5529
 * @version 0.1
 * @date 2020-12-12
 *
 * @copyright Copyright (c) 2020 DSS - LORIS project
 *
 * @note
 * @todo
 */

#if !defined(TARGET_MCU)
#error DRIVER COMPILATION SHOULD ONLY OCCUR ON CROSSCOMPILED TARGETS
#endif /* !defined(TARGET_MCU) */

#include <msp430.h>

#include "targets.h"

void watchdog_stop(void)
{
    WDTCTL = WDTPW | WDTHOLD; /* stop watchdog timer */
}


void watchdog_start(void)
{
    WDTCTL &= ~(WDTPW | WDTHOLD);
}


#warning watchdog_kick not yet implemented
void watchdog_kick(void)
{}

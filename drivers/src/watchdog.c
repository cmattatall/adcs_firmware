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
#include "platform.h"

void watchdog_stop(void)
{
#if defined(TARGET_MCU)
    WDTCTL = WDTPW | WDTHOLD; /* stop watchdog timer */
#else
    log_trace("stopped watchdog\n");
#endif /* #if defined(TARGET_MCU) */
}


void watchdog_start(void)
{
#if defined(TARGET_MCU)
    WDTCTL &= ~(WDTPW | WDTHOLD);
#else
    log_trace("started watchdog\n");
#endif /* #if defined(TARGET_MCU) */
}


void watchdog_kick(void)
{
#if defined(TARGET_MCU)
#warning watchdog_kick not yet implemented
#else
    log_trace("kicked watchdog\n");
#endif /* #if defined(TARGET_MCU) */
}

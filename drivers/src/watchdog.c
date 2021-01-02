/**
 * @file watchdog.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to wrap dependencies for watchdog timer on msp430f5529
 * @version 0.1
 * @date 2020-12-12
 *
 * @copyright Copyright (c) 2020 DSS - LORIS project
 *
 */
#include <stdint.h>


#if !defined(TARGET_MCU)
#error DRIVER COMPILATION SHOULD ONLY OCCUR ON CROSSCOMPILED TARGETS
#endif /* !defined(TARGET_MCU) */

#include <msp430.h>

#include "targets.h"

#define WATCHDOG_WRITE_PASSWORD ((uint16_t)(0x005a << 8))
#define WATCHDOG_READ_PASSWORD ((uint16_t)(0x69 << 8))


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
/* According to second 16.2, of slau208q, any writes that do not have
 * 0x5a in upper byte generate PUC? But this is most certainly not the
 * experience of everyone working on the board */
static void write_watchdog(uint16_t val)
{
    val |= WATCHDOG_WRITE_PASSWORD;
    WDTCTL = val;
}

#pragma GCC diagnostic pop


void watchdog_stop(void)
{
    WDTCTL = WDTPW | WDTHOLD; /* stop watchdog timer */
}


void watchdog_start(void)
{
    WDTCTL = WDT_MRST_32;
}


void watchdog_kick(void)
{
    WDTCTL |= WDTCNTCL;
}

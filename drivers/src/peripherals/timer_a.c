/**
 * @file timers.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Driver file for timers on MSP430F5529
 * @version 0.1
 * @date 2020-12-19
 *
 * @copyright Copyright (c) 2020 DSS Loris project
 *
 */
#if defined(TARGET_MCU)

#include <msp430.h>
#include <stdlib.h>
#include <limits.h>

#include "clocks.h"
#include "timer_a.h"


/* clang-format on */

#else

#error DRIVER COMPILATION SHOULD ONLY OCCUR ON CROSSCOMPILED TARGETS

#endif /* !defined(TARGET_MCU) */

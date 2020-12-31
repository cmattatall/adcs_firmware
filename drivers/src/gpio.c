/**
 * @file gpio.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to provide a peripheral interface for the
 * gpio pins
 * @version 0.1
 * @date 2020-12-31
 *
 * @copyright Copyright (c) 2020 LORIS PROJECT - DSS
 *
 * @note
 * @todo IMPLEMENT THE STUFF
 */

#include <msp430.h>
#include "gpio.h"

#if !defined(TARGET_MCU)
#error DRIVER COMPILATION SHOULD ONLY OCCUR ON CROSSCOMPILED TARGETS
#endif /* !defined(TARGET_MCU) */

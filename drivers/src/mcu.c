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
#include <msp430.h>

#include "mcu.h"
#include "targets.h"


void enable_interrupts(void)
{
    //__bis_SR_register(LPM0_bits + GIE); /* Enter LPM0, interrupts enabled */
    __bis_SR_register(GIE);
}
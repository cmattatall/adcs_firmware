/**
 * @file rw_z.example.c
 * @author thomas christison (christisonthomas@gmail.com)
 * @brief Single file executable to demonstrate driving maxon motor 
 * using a pwm signal(z == P2.5) and direction GPIO
 * using register manipulation
 * @version 0.1
 * @date 2021-05-23
 *
 * @copyright Copyright (c) 2021 Thomas Christison
 *
 * PINOUT:
 * P2.5 == RW_Z_SPEED_CTRL (TA2.2)
 * PJ.1 == RW_Z_DIR
 * 
 * Motor Driver: Texas Instruments DRV10970
 * https://www.ti.com/lit/ds/symlink/drv10970.pdf?ts=1617147100144&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FDRV10970 
 * 
 * Tachometer: Texas Instruments LM2917N-8/NOPB
 * https://www.ti.com/lit/ds/snas555d/snas555d.pdf
 * Tachometer output to channel CH3 of U2 (CURR_SENSE_MAGNETORQUER) in test setup (breadboard)
 * 
 */

#if defined(TARGET_MCU)
#include <msp430.h>
#include "timer_a.h"
#else
#endif /* #if defined(TARGET_MCU) */

#define API_TIMER_OUTMOD_INIT_MODE (OUTMOD_TOG_SET)
#define API_TIMER_MC_INIT_MODE (MC__CONTINUOUS)
#define API_TIMER_TASSEL_INIT_MODE (TASSEL__SMCLK)


int main()
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

    /* Motherboard Indicator LED (P6.0)*/
    P6DIR |= BIT0;
    P6OUT |= BIT0;

    /* PIN P2.5 (RW_Z_SPEED_CTRL) */
    P2DIR |= BIT5;
    P2SEL |= BIT5;

    /* PIN PJ.1 (RW_Z_DIR) */
    PJDIR |= BIT1; // Set PJ.1 to output direction
    PJOUT = 1;

    /* Stop timer A2 */
    TA2CTL &= ~(MC0 | MC1); 

    /* Confgigure pwm on pin 2.5 (TA.2.2) */
    TA2CCR2 = 0;
    TA2CCTL2 &= ~(OUTMOD0 | OUTMOD1 | OUTMOD2);
    TA2CCTL2 |= OUTMOD_TOG_SET;

    /* Set Timer A2 clock source to smclk */
    TA2CTL &= ~(TASSEL0 | TASSEL1);
    TA2CTL |= TASSEL__SMCLK;

    /* Set count mode */
    TA2CTL &= ~(MC0 | MC1);

    /* start timerA2 count from 0 to ffff forever */
    TA2CTL |= MC__CONTINOUS;

    TB0CTL &= ~(MC0 | MC1); /* Stop timer B0 */
    TB0CTL &= ~(TBSSEL0 | TBSSEL1);
    TB0CTL |= TBSSEL__SMCLK;

    TB0CCR5 = 0;
    TB0CCTL5 &= ~(OUTMOD0 | OUTMOD1 | OUTMOD2);
    TB0CCTL5 |= OUTMOD_TOG_SET;
    TB0CTL |= MC__CONTINOUS;

    TA2CCR2 = UINT16_MAX / 2.0f; /* 50% DUTY cycle on P2.5 */

    _BIS_SR(GIE + LPM0_bits);

    while (1)
    {
    }
}
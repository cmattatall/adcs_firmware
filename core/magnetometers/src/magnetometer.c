/**
 * @file sun_sensors.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to implement the sun sensor interface
 * @version 0.1
 * @date 2021-01-01
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 *
 * @note PINOUT:
 *  P2.7 == SPI CHIP SELECT
 *
 * @todo BARE METAL REGISTER WRITES SHOULD BE ABSTRACTED INTO DRIVER LAYER
 */

#include <stdlib.h>
#include <string.h>

#include "attributes.h"
#include "config_assert.h"
#include "magnetometer.h"

#if defined(TARGET_MCU)
#include "spi.h"
#include "ads7841e.h"
#include <msp430.h>
#else
#include <stdio.h>
#endif /* #if defined(TARGET_MCU) */

typedef struct
{
    int structure_fields_to_change_as_needed;
} MAGTOM_measurement_t;

static void MAGTOM_enable_ADS7841(void);
static void MAGTOM_disable_ADS7841(void);
static void MAGTOM_init_phy(void);

static MAGTOM_measurement_t MAGTOM_get_measurement(void);




void MAGTOM_reset(void)
{
#if defined(TARGET_MCU)
    /* Issue reset command to IMU */
#warning NOT IMPLEMENTED YET
#else
    printf("Called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


int MAGTOM_measurement_to_string(char *buf, int buflen)
{
    CONFIG_ASSERT(NULL != buf);
    MAGTOM_measurement_t measurement     = MAGTOM_get_measurement();
    int                  required_length = 0;

/** @todo IMPLEMENT */
#warning STRING FORMAT FOR MAGNETOMETER MEASUREMENT NOT IMLPEMENTED YET BECAUSE WE DONT KNOW HOW TO FULLY USE THE BNO055 API YET

    return (required_length < buflen) ? 0 : 1;
}


static void MAGTOM_enable_ADS7841(void)
{
#if defined(TARGET_MCU)

    P2OUT &= ~BIT7;

#else
    printf("Called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void MAGTOM_disable_ADS7841(void)
{
#if defined(TARGET_MCU)

    P2OUT |= BIT7;

#else
    printf("Called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static MAGTOM_measurement_t MAGTOM_get_measurement(void)
{
    MAGTOM_measurement_t data = {0};
    MAGTOM_init_phy();
#if defined(TARGET_MCU)
    ADS7841_driver_init(MAGTOM_enable_ADS7841, MAGTOM_disable_ADS7841,
                        ADS7841_PWRMODE_stayOn, ADS7841_BITRES_12);

    /*
     * Measure the conversion value (singled ended mode) from channel 2 on the
     * ADS7841 chip that is selected using functions MAGTOM_enable_ADS7841 and
     * MAGTOM_disable_ADS7841
     */
    data.structure_fields_to_change_as_needed =
        ADS7841_measure_channel(ADS7841_CHANNEL_SGL_2);
    ADS7841_driver_deinit();
#else
    printf("Called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
    return data;
}


static void MAGTOM_init_phy(void)
{
#if defined(TARGET_MCU)

    /* De-select ADS7841 for the magnetometer */
    P2DIR |= BIT7;
    P2OUT |= BIT7;

#else
    printf("Called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}
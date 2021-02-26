/**
 * @file sun_sensors.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to implement the sun sensor interface
 * @version 0.1
 * @date 2021-01-01
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 * @todo IMPLEMENT THE INTERFACE
 */

#include <stdlib.h>
#include <string.h>

#include "attributes.h"
#include "config_assert.h"
#include "magnetometer.h"

#if defined(TARGET_MCU)
#include "spi.h"
#include "ads7841e.h"
#else
#include <stdio.h>
#endif /* #if defined(TARGET_MCU) */

static void MAGTOM_enable_ADS7841(void);
static void MAGTOM_disable_ADS7841(void);

void MAGTOM_reset(void)
{
#if defined(TARGET_MCU)
    /* Issue reset command to IMU */
#warning NOT IMPLEMENTED YET
#else
    printf("Called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


MAGTOM_measurement_t MAGTOM_get_measurement(void)
{
    MAGTOM_measurement_t data = {0};
#if defined(TARGET_MCU)


    ADS7841_driver_init(MAGTOM_enable_ADS7841, MAGTOM_disable_ADS7841,
                        ADS7841_PWRMODE_stayOn, ADS7841_BITRES_12);

    /* Measure the conversion value (singled ended mode) from channel 2 on the
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


static void MAGTOM_enable_ADS7841(void)
{
#if defined(TARGET_MCU)

    /* THIS FUNCTION SHOULD SET GPIO PIN 2.7 HIGH */

    /** @todo SET SPI CS MUX TO DRIVE CS PIN OF ADS7841 ON magnetometer LOW */

#warning NOT IMPLEMENTED YET
#else
    printf("Called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void MAGTOM_disable_ADS7841(void)
{
#if defined(TARGET_MCU)
#warning NOT IMPLEMENTED YET

    /* THIS FUNCTION SHOULD SET GPIO PIN 2.7 LOW */

    /** @todo SET SPI CS MUX TO DRIVE CS PIN OF ADS7841 ON magnetometer HIGH */
#else
    printf("Called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}

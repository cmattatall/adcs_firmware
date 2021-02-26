/**
 * @file sun_sensors.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to implement the sun sensor interface
 * @version 0.2
 * @date 2021-01-01
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 * @todo IMPLEMENT THE FUNCTIONS TO ENABLE THE CS PINS ON THE VARIOUS ADS7841
 * ICS
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "attributes.h"
#include "config_assert.h"
#include "sun_sensors.h"

#if defined(TARGET_MCU)
#include "spi.h"
#include "ads7841e.h"
#else
#endif /* #if defined(TARGET_MCU) */

typedef struct
{
    float lux_1;
    float lux_2;
    float lux_3;
} SUNSEN_measurement_t;

static void SUNSEN_enable_ADS7841_x_plus(void);
static void SUNSEN_enable_ADS7841_x_minus(void);
static void SUNSEN_enable_ADS7841_y_plus(void);
static void SUNSEN_enable_ADS7841_y_minus(void);
static void SUNSEN_enable_ADS7841_z_plus(void);
static void SUNSEN_enable_ADS7841_z_minus(void);

static void SUNSEN_disable_ADS7841_x_plus(void);
static void SUNSEN_disable_ADS7841_x_minus(void);
static void SUNSEN_disable_ADS7841_y_plus(void);
static void SUNSEN_disable_ADS7841_y_minus(void);
static void SUNSEN_disable_ADS7841_z_plus(void);
static void SUNSEN_disable_ADS7841_z_minus(void);


static SUNSEN_measurement_t SUNSEN_get_face_lux(SUNSEN_FACE_t face);
static int                  SUNSEN_adcs_to_temp_deg_c(uint16_t adc_val);


static void (*SUNSEN_enable_functions[])(void) = {
    [SUNSEN_FACE_x_pos] = SUNSEN_enable_ADS7841_x_plus,
    [SUNSEN_FACE_x_neg] = SUNSEN_enable_ADS7841_x_minus,
    [SUNSEN_FACE_y_pos] = SUNSEN_enable_ADS7841_y_plus,
    [SUNSEN_FACE_y_neg] = SUNSEN_enable_ADS7841_y_minus,
    [SUNSEN_FACE_z_pos] = SUNSEN_enable_ADS7841_z_plus,
    [SUNSEN_FACE_z_neg] = SUNSEN_enable_ADS7841_z_minus,
};

static void (*SUNSEN_disable_functions[])(void) = {
    [SUNSEN_FACE_x_pos] = SUNSEN_disable_ADS7841_x_plus,
    [SUNSEN_FACE_x_neg] = SUNSEN_disable_ADS7841_x_minus,
    [SUNSEN_FACE_y_pos] = SUNSEN_disable_ADS7841_y_plus,
    [SUNSEN_FACE_y_neg] = SUNSEN_disable_ADS7841_y_minus,
    [SUNSEN_FACE_z_pos] = SUNSEN_disable_ADS7841_z_plus,
    [SUNSEN_FACE_z_neg] = SUNSEN_disable_ADS7841_z_minus,
};


int SUNSEN_face_lux_to_string(char *buf, unsigned int len, SUNSEN_FACE_t face)
{
    CONFIG_ASSERT(NULL != buf);
    SUNSEN_measurement_t m = SUNSEN_get_face_lux(face);
    int                  req; /* required length to fill message buffer */
    req = snprintf(buf, len, "[ %.3f, %.3f, %.3f ]", m.lux_1, m.lux_2, m.lux_3);
    return (((unsigned int)req) < len) ? 0 : 1;
}


int SUNSEN_get_z_pos_temp(void)
{
    int adc_val = 0;
    ADS7841_driver_init(SUNSEN_enable_functions[SUNSEN_FACE_z_pos],
                        SUNSEN_disable_functions[SUNSEN_FACE_z_pos],
                        ADS7841_PWRMODE_stayOn, ADS7841_BITRES_12);
    adc_val = ADS7841_measure_channel(ADS7841_CHANNEL_SGL_0);

    ADS7841_driver_deinit();
    return SUNSEN_adcs_to_temp_deg_c(adc_val);
}


int SUNSEN_get_z_neg_temp(void)
{
    int adc_val = 0;
    ADS7841_driver_init(SUNSEN_enable_functions[SUNSEN_FACE_z_neg],
                        SUNSEN_disable_functions[SUNSEN_FACE_z_neg],
                        ADS7841_PWRMODE_stayOn, ADS7841_BITRES_12);
    adc_val = ADS7841_measure_channel(ADS7841_CHANNEL_SGL_0);
    ADS7841_driver_deinit();
    return SUNSEN_adcs_to_temp_deg_c(adc_val);
}


static void SUNSEN_enable_ADS7841_x_plus(void)
{
#if defined(TARGET_MCU)

    /** @todo SET SPI CS MUX TO DRIVE CS PIN OF ADS7841 ON SUN SENSOR LOW */

#warning NOT IMPLEMENTED YET
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_enable_ADS7841_x_minus(void)
{
#if defined(TARGET_MCU)

    /** @todo SET SPI CS MUX TO DRIVE CS PIN OF ADS7841 ON SUN SENSOR LOW */

#warning NOT IMPLEMENTED YET
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_enable_ADS7841_y_plus(void)
{
#if defined(TARGET_MCU)

    /** @todo SET SPI CS MUX TO DRIVE CS PIN OF ADS7841 ON SUN SENSOR LOW */

#warning NOT IMPLEMENTED YET
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_enable_ADS7841_y_minus(void)
{
#if defined(TARGET_MCU)

    /** @todo SET SPI CS MUX TO DRIVE CS PIN OF ADS7841 ON SUN SENSOR LOW */

#warning NOT IMPLEMENTED YET
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_enable_ADS7841_z_plus(void)
{
#if defined(TARGET_MCU)

    /** @todo SET SPI CS MUX TO DRIVE CS PIN OF ADS7841 ON SUN SENSOR LOW */

#warning NOT IMPLEMENTED YET
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_enable_ADS7841_z_minus(void)
{
#if defined(TARGET_MCU)

    /** @todo SET SPI CS MUX TO DRIVE CS PIN OF ADS7841 ON SUN SENSOR LOW */

#warning NOT IMPLEMENTED YET
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_disable_ADS7841_x_plus(void)
{
#if defined(TARGET_MCU)

    /** @todo SET SPI CS MUX TO DRIVE CS PIN OF ADS7841 ON SUN SENSOR LOW */

#warning NOT IMPLEMENTED YET
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_disable_ADS7841_x_minus(void)
{
#if defined(TARGET_MCU)

    /** @todo SET SPI CS MUX TO DRIVE CS PIN OF ADS7841 ON SUN SENSOR LOW */

#warning NOT IMPLEMENTED YET
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_disable_ADS7841_y_plus(void)
{
#if defined(TARGET_MCU)

    /** @todo SET SPI CS MUX TO DRIVE CS PIN OF ADS7841 ON SUN SENSOR LOW */

#warning NOT IMPLEMENTED YET
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_disable_ADS7841_y_minus(void)
{
#if defined(TARGET_MCU)

    /** @todo SET SPI CS MUX TO DRIVE CS PIN OF ADS7841 ON SUN SENSOR LOW */

#warning NOT IMPLEMENTED YET
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_disable_ADS7841_z_plus(void)
{
#if defined(TARGET_MCU)

    /** @todo SET SPI CS MUX TO DRIVE CS PIN OF ADS7841 ON SUN SENSOR LOW */

#warning NOT IMPLEMENTED YET
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_disable_ADS7841_z_minus(void)
{
#if defined(TARGET_MCU)

    /** @todo SET SPI CS MUX TO DRIVE CS PIN OF ADS7841 ON SUN SENSOR LOW */

#warning NOT IMPLEMENTED YET
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static SUNSEN_measurement_t SUNSEN_get_face_lux(SUNSEN_FACE_t face)
{
    SUNSEN_measurement_t measurement;
    memset(&measurement, 0, sizeof(measurement));
#if defined(TARGET_MCU)
    ADS7841_driver_init(SUNSEN_enable_functions[face],
                        SUNSEN_disable_functions[face], ADS7841_PWRMODE_stayOn,
                        ADS7841_BITRES_12);
    measurement.lux_1 = ADS7841_measure_channel(ADS7841_CHANNEL_SGL_1);
    measurement.lux_2 = ADS7841_measure_channel(ADS7841_CHANNEL_SGL_2);
    measurement.lux_3 = ADS7841_measure_channel(ADS7841_CHANNEL_SGL_3);
    ADS7841_driver_deinit();
#else
    printf("Called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
    return measurement;
}


static int SUNSEN_adcs_to_temp_deg_c(uint16_t adc_val)
{
#warning NOT IMPLEMENTED YET
    int deg_c = 50;

#if defined(TARGET_MCU)


#else
    printf("called %s with retval == %d\n", __func__, deg_c);
#endif /* #if defined(TARGET_MCU) */
    return deg_c;
}

/**
 * @file sun_sensors.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to implement the sun sensor interface
 * @version 0.2
 * @date 2021-01-01
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 * PINOUT:
 * P4.3 : CS_SUN_X+
 * P4.2 : CS_SUN_Y+
 * P4.1 : CS_SUN_Z+
 *
 * P8.0 : CS_SUN_X-
 * P8.1 : CS_SUN_Y-
 * P8.2 : CS_SUN_Z-
 *
 * 
 * @todo DIRECT CALLS TO REGISTER LEVEL SHOULD BE ABSTRACTED INTO DRIVER LAYER
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "attributes.h"
#include "config_assert.h"
#include "sun_sensors.h"

#if defined(TARGET_MCU)
#include <msp430.h>
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


void SUNSEN_init_phy(void)
{
    /** @note
     * CLOCK SELECT PINOUTS
     * P4.3 : CS_SUN_X+
     * P4.2 : CS_SUN_Y+
     * P4.1 : CS_SUN_Z+
     *
     * P8.0 : CS_SUN_X-
     * P8.1 : CS_SUN_Y-
     * P8.2 : CS_SUN_Z-
     * 
     * (Levels initialized to high because CS is active low)
     */
    P4DIR |= BIT3; /* X+ */
    P4OUT |= BIT3;

    P4DIR |= BIT2; /* Y+ */
    P4OUT |= BIT2;

    P4DIR |= BIT1; /* Z+ */
    P4OUT |= BIT1;

    P8DIR |= BIT0; /* X- */
    P8OUT |= BIT0;

    P8DIR |= BIT1; /* Y- */
    P8OUT |= BIT1;

    P8DIR |= BIT2; /* Z- */
    P8OUT |= BIT2;
}


int SUNSEN_face_lux_to_string(char *buf, int len, SUNSEN_FACE_t face)
{
    CONFIG_ASSERT(NULL != buf);
    SUNSEN_measurement_t m = SUNSEN_get_face_lux(face);
    int                  req; /* required length to fill message buffer */
    req = snprintf(buf, len, "[ %.3f, %.3f, %.3f ]", m.lux_1, m.lux_2, m.lux_3);
    return (req < len) ? 0 : 1;
}


int SUNSEN_get_z_pos_temp(void)
{
    int adc_val = 0;
#if defined(TARGET_MCU)
    ADS7841_driver_init(SUNSEN_enable_functions[SUNSEN_FACE_z_pos],
                        SUNSEN_disable_functions[SUNSEN_FACE_z_pos],
                        ADS7841_PWRMODE_stayOn, ADS7841_BITRES_12);
    adc_val = ADS7841_measure_channel(ADS7841_CHANNEL_SGL_0);

    ADS7841_driver_deinit();
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
    return SUNSEN_adcs_to_temp_deg_c(adc_val);
}


int SUNSEN_get_z_neg_temp(void)
{
    int adc_val = 0;
#if defined(TARGET_MCU)
    ADS7841_driver_init(SUNSEN_enable_functions[SUNSEN_FACE_z_neg],
                        SUNSEN_disable_functions[SUNSEN_FACE_z_neg],
                        ADS7841_PWRMODE_stayOn, ADS7841_BITRES_12);
    adc_val = ADS7841_measure_channel(ADS7841_CHANNEL_SGL_0);
    ADS7841_driver_deinit();
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
    return SUNSEN_adcs_to_temp_deg_c(adc_val);
}


static void SUNSEN_enable_ADS7841_x_plus(void)
{
#if defined(TARGET_MCU)
    P4OUT &= ~BIT3;
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_enable_ADS7841_x_minus(void)
{
#if defined(TARGET_MCU)
    P8OUT &= ~BIT0;
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_enable_ADS7841_y_plus(void)
{
#if defined(TARGET_MCU)
    P4OUT &= ~BIT2;
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_enable_ADS7841_y_minus(void)
{
#if defined(TARGET_MCU)
    P8OUT &= ~BIT1;
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_enable_ADS7841_z_plus(void)
{
#if defined(TARGET_MCU)
    P4OUT &= ~BIT1;
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_enable_ADS7841_z_minus(void)
{
#if defined(TARGET_MCU)
    P8OUT &= ~BIT2;
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_disable_ADS7841_x_plus(void)
{
#if defined(TARGET_MCU)
    P4OUT |= BIT3;
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_disable_ADS7841_x_minus(void)
{
#if defined(TARGET_MCU)
    P8OUT |= BIT0;
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_disable_ADS7841_y_plus(void)
{
#if defined(TARGET_MCU)
    P4OUT |= BIT2;
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_disable_ADS7841_y_minus(void)
{
#if defined(TARGET_MCU)
    P8OUT |= BIT1;
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_disable_ADS7841_z_plus(void)
{
#if defined(TARGET_MCU)
    P4OUT |= BIT1;
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


static void SUNSEN_disable_ADS7841_z_minus(void)
{
#if defined(TARGET_MCU)
    P8OUT |= BIT2;
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

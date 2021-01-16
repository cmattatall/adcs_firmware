/**
 * @file ads7841e.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module for driver layer to ADS7841e ADC using SPI connection
 * @version 0.1
 * @date 2021-01-08
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 * @note THIS MODULE EXPLICITLY DOES NOT USE BITFIELDS BECAUSE THE
 * __ANCIENT__ compiler that TI uses has known issues with the assembly it
 * emits regarding padding / ordering of bitfields. PLEASE PLEASE PLEASE
 * do not rewrite this to be clever and use bitfields
 *
 *  - also, on a related note, as of C11, bitfield ordering is still compiler
 *  and not target endianness dependent...
 *
 * PINOUT:
 * *************************************
 * MSP430                   SUN_SENSOR
 * 5V --------------------------- 5V
 * GND -------------------------- GND
 * P3.0 (UCB0 MOSI) ------------- MOSI
 * P3.1 (UCB0 MISO) ------------- MISO
 * P3.2 (UCB0CLK) --------------- CLK
 * P2.3 (SPICS_other)------------ CS
 * 3V3 -------------------------- 3V3
 */

#if !defined(TARGET_MCU)
#error DRIVER COMPILATION SHOULD ONLY OCCUR ON CROSSCOMPILED TARGETS
#endif /* !defined(TARGET_MCU) */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "config_assert.h"
#include "ads7841e.h"
#include "bufferlib.h"
#include "spi.h"

#define CTL_START_POS (7u)
#define CTL_START_MSK (1u << (CTL_START_POS))

#define CTL_CHANNEL_POS (4u)
#define CTL_CHANNEL_MSK ((7u) << (CTL_CHANNEL_POS))

#define CTL_MODE_POS (3u)
#define CTL_MODE_MSK (1u << (CTL_MODE_POS))
#define CTL_MODE_8BIT ((1u << (CTL_MODE_POS)) & (CTL_MODE_MSK))
#define CTL_MODE_12BIT ((0u << (CTL_MODE_POS)) & (CTL_MODE_MSK))

#define CTL_SGL_POS (2u)
#define CTL_SGL_MSK (1u << (CTL_SGL_POS))
#define CTL_SGL_CONVERSION_SINGLE_ENDED ((1u << (CTL_SGL_POS)) & (CTL_SGL_MSK))
#define CTL_SGL_CONVERSION_DIFFERENTIAL ((0u << (CTL_SGL_POS)) & (CTL_SGL_MSK))

#define CTL_PWRMODE_POS (0)
#define CTL_PWRMODE_MSK (2u << (CTL_PWRMODE_POS))

#define ADS7841_BITMASK12 (0x0FFF) /* 12 bit mask */
#define ADS7841_BITMASK8 (0x00FF)  /* 8 bit mask  */

#if defined(ADS7841_OVERSAMPLE_COUNT)
#warning ADS7841_OVERSAMPLE_COUNT is being overridden!
#else
#define ADS7841_OVERSAMPLE_COUNT 10
#endif /* #if defined(ADS7841_OVERSAMPLE_COUNT) */

static struct
{
    ADS7841_PWRMODE_t  power_mode;
    ADS7841_CONVMODE_t conv_type;
    uint16_t           sample_mask;
} ADS7841_cfg;


static volatile uint8_t      conv_bytes[2];
static volatile unsigned int conv_cnt;
static volatile uint16_t     conv_samples[ADS7841_OVERSAMPLE_COUNT];
static volatile unsigned int conv_byte_idx = 0;

static void (*ADS7841_SPI_CHIP_SELECT_func)(void)   = NULL;
static void (*ADS7841_SPI_CHIP_UNSELECT_func)(void) = NULL;


static union
{
    uint8_t  bytes[2];
    uint16_t conv_val;
} ADS7841_sample_holder;


static uint8_t ADS7841_ctrl_byte(ADS7841_CHANNEL_t  channel,
                                 ADS7841_PWRMODE_t  mode,
                                 ADS7841_CONVMODE_t conv_type);

static void ADS7841_receive_byte(uint8_t byte);
static int  ADS7841_conv_SINGLE(ADS7841_CHANNEL_t ch, ADS7841_CONVMODE_t type);


/* See table 1 of page 9 datasheet.
 * I have NO clue why they chose a bit mapping like this... */
static const uint8_t ADS7841_CHANNEL_MAP[] = {
    [ADS7841_CHANNEL_0] = (1 << CTL_CHANNEL_POS) & CTL_CHANNEL_MSK,
    [ADS7841_CHANNEL_1] = (5 << CTL_CHANNEL_POS) & CTL_CHANNEL_MSK,
    [ADS7841_CHANNEL_2] = (2 << CTL_CHANNEL_POS) & CTL_CHANNEL_MSK,
    [ADS7841_CHANNEL_3] = (6 << CTL_CHANNEL_POS) & CTL_CHANNEL_MSK,
};

/* The manufacturer has a similarly idiotic bit layout for the power modes... */
static const uint8_t ADS7841_PWRMODE_MAP[] = {
    [ADS7841_PWRMODE_inter_conv] = ((0 << CTL_PWRMODE_POS) & CTL_PWRMODE_MSK),
    [ADS7841_PWRMODE_always_on]  = ((3 << CTL_PWRMODE_POS) & CTL_PWRMODE_MSK),
};

void ADS7841_driver_init(void (*ena_func)(void), void (*dis_func)(void),
                         ADS7841_PWRMODE_t mode, ADS7841_CONVMODE_t conv_type)
{
    ADS7841_cfg.power_mode = mode;
    switch (conv_type)
    {
        case ADS7841_CONVMODE_8:
        {
            ADS7841_cfg.sample_mask = ADS7841_BITMASK8;
        }
        break;
        case ADS7841_CONVMODE_12:
        {
            ADS7841_cfg.sample_mask = ADS7841_BITMASK12;
        }
        break;
        default:
        {
            ADS7841_cfg.sample_mask = ADS7841_BITMASK12;
        }
        break;
    }
    ADS7841_SPI_CHIP_SELECT_func   = ena_func;
    ADS7841_SPI_CHIP_UNSELECT_func = dis_func;
    SPI0_init(ADS7841_receive_byte, SPI_DATA_DIR_msb, SPI_MODE_async);
}


void ADS7841_driver_deinit(void)
{
    ADS7841_SPI_CHIP_SELECT_func   = NULL;
    ADS7841_SPI_CHIP_UNSELECT_func = NULL;
    SPI0_deinit();
}


void ADS7841_TEST(void)
{
    ADS7841_SPI_CHIP_SELECT_func();
    ADS7841_conv_SINGLE(ADS7841_CHANNEL_3, ADS7841_CONVMODE_12);
    ADS7841_SPI_CHIP_UNSELECT_func();

#if 0
    uint16_t val;
    val = ADS7841_measure_channel(ADS7841_CHANNEL_3);
    if (val != ADS7841_CONV_STATUS_BUSY)
    {
        /* Do stuff with the data */
    }

#endif
}

static uint8_t ADS7841_ctrl_byte(ADS7841_CHANNEL_t  channel,
                                 ADS7841_PWRMODE_t  mode,
                                 ADS7841_CONVMODE_t conv_type)
{
    uint8_t control_byte = 0;

    switch (channel)
    {
        case ADS7841_CHANNEL_0:
        case ADS7841_CHANNEL_1:
        case ADS7841_CHANNEL_2:
        case ADS7841_CHANNEL_3:
        {
            control_byte |= ADS7841_CHANNEL_MAP[channel];
        }
        break;
        default:
        {
            control_byte &= ~(CTL_CHANNEL_MSK);
        }
        break;
    }

    switch (mode)
    {
        case ADS7841_PWRMODE_inter_conv:
        case ADS7841_PWRMODE_always_on:
        {
            control_byte |= ADS7841_PWRMODE_MAP[mode];
        }
        break;
        default:
        {
            control_byte &= ~(CTL_PWRMODE_MSK);
        }
    }

    /* We're only going to do single-ended conversions so just force
     * the control byte into single mode */
    control_byte |= CTL_SGL_CONVERSION_SINGLE_ENDED;

    /** @note this is a bit hacky but whatever */
    switch (conv_type)
    {
        case ADS7841_CONVMODE_8:
        {
            control_byte |= CTL_MODE_8BIT;
        }
        break;
        case ADS7841_CONVMODE_12:
        {
            control_byte |= CTL_MODE_12BIT;
        }
        break;
        default:
        {
            control_byte &= ~(CTL_MODE_MSK);
        }
        break;
    }


    control_byte |= ((1 << CTL_START_POS) & CTL_START_MSK);

    return control_byte;
}


static unsigned int ADS7841_busy;
uint16_t            ADS7841_measure_channel(ADS7841_CHANNEL_t ch)
{
    uint16_t conversion_value = ADS7841_CONV_STATUS_BUSY;
    if (!ADS7841_busy)
    {
        CONFIG_ASSERT(ADS7841_SPI_CHIP_SELECT_func != NULL);
        CONFIG_ASSERT(ADS7841_SPI_CHIP_UNSELECT_func != NULL);
        ADS7841_SPI_CHIP_SELECT_func();
        ADS7841_busy = 1;

        /* Perform the required number of samples */
        for (conv_cnt = 0; conv_cnt < ADS7841_OVERSAMPLE_COUNT; conv_cnt++)
        {
            ADS7841_conv_SINGLE(ch, ADS7841_cfg.conv_type);
        }

        ADS7841_SPI_CHIP_UNSELECT_func();

        /* Compute average value from set of samples */
        conversion_value = 0;
        while (conv_cnt)
        {
            conversion_value += conv_samples[conv_cnt];
            conv_cnt--;
        }
        conversion_value /= ADS7841_OVERSAMPLE_COUNT;
    }
    return conversion_value;
}


static void ADS7841_receive_byte(uint8_t byte)
{
    if (conv_byte_idx == 0)
    {
        ADS7841_sample_holder.bytes[conv_byte_idx] = byte;
        conv_byte_idx                              = 1;
    }
    else
    {
        ADS7841_sample_holder.bytes[conv_byte_idx] = byte;
        conv_byte_idx                              = 0;

        if (conv_cnt < ADS7841_OVERSAMPLE_COUNT)
        {
            ADS7841_sample_holder.conv_val &= ADS7841_cfg.sample_mask;
            conv_samples[conv_cnt] = ADS7841_sample_holder.conv_val;
            conv_cnt++;
        }
    }
}


static void ADS7841_transmit_delay(void)
{
    volatile unsigned int i;
    for (i = 0; i < 1000; i++)
    {
        /* Force inter-clock delay */
    }
}

static int ADS7841_conv_SINGLE(ADS7841_CHANNEL_t ch, ADS7841_CONVMODE_t type)
{
    uint16_t power_mode = ADS7841_cfg.power_mode;
    uint8_t  ctrl_byte  = ADS7841_ctrl_byte(ch, power_mode, type);
    uint8_t  msg[]      = {ctrl_byte, '\0'};
    uint16_t msglen     = (uint16_t)(sizeof(msg) / sizeof(*msg));
    return SPI0_transmit(msg, msglen, ADS7841_transmit_delay);
}

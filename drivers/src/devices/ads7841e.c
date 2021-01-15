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


#define ADS7841_OVERSAMPLE_COUNT 50

static struct
{
    ADS7841_PWRMODE_t  power_mode;
    ADS7841_CONVTYPE_t conv_type;
    uint16_t           sample_mask;
} ADS7841_cfg;


static volatile uint8_t      conv_bytes[2];
static volatile unsigned int conv_sample_cnt;
static volatile uint16_t     conv_samples[ADS7841_OVERSAMPLE_COUNT];

static union
{
    uint8_t  bytes[2];
    uint16_t conv_val;
} ADS7841_sample_holder;


static uint8_t ADS7841_ctrl_byte(ADS7841_CHANNEL_t  channel,
                                 ADS7841_PWRMODE_t  mode,
                                 ADS7841_CONVTYPE_t conv_type);

static void ADS7841_receive_byte_internal(uint8_t byte);

static void ADS7841_get_conv_SINGLE(ADS7841_CHANNEL_t  ch,
                                    ADS7841_CONVTYPE_t conv_type);


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


void ADS7841_driver_init(ADS7841_PWRMODE_t mode, ADS7841_CONVTYPE_t conv_type)
{
    ADS7841_cfg.power_mode = mode;
    switch (conv_type)
    {
        case ADS7841_CONVTYPE_8:
        {
            ADS7841_cfg.sample_mask = ADS7841_BITMASK8;
        }
        break;
        case ADS7841_CONVTYPE_12:
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
    SPI0_init(ADS7841_receive_byte_internal, SPI_DIR_msb, SPI_MODE_async);
}

void ADS7841_driver_deinit(void)
{
    SPI0_deinit();
}


static uint8_t ADS7841_ctrl_byte(ADS7841_CHANNEL_t  channel,
                                 ADS7841_PWRMODE_t  mode,
                                 ADS7841_CONVTYPE_t conv_type)
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
        case ADS7841_CONVTYPE_8:
        {
            control_byte |= CTL_MODE_8BIT;
        }
        break;
        case ADS7841_CONVTYPE_12:
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


uint16_t ADS7841_get_conv(ADS7841_CHANNEL_t ch)
{
    uint16_t conversion_value = ADS7841_CONV_STATUS_BUSY;
    if (conv_sample_cnt == 0)
    {
        unsigned int sample_idx;
        for (sample_idx = 0; sample_idx < ADS7841_OVERSAMPLE_COUNT;
             sample_idx++)
        {
            ADS7841_get_conv_SINGLE(ch, ADS7841_cfg.conv_type);
        }

        /* Wait for oversampling to complete */
        volatile uint16_t timeout = 10000;
        while (conv_sample_cnt >= ADS7841_OVERSAMPLE_COUNT && --timeout)
        {
            /** @note This is done as a noise mitigation measure */
        }

        if (timeout)
        {
            /* Samples didn't complete during conversion timeout window */
            conversion_value = ADS7841_CONV_STATUS_ERROR;
        }
        else
        {
            /* Compute average value from set of samples */
            conversion_value = 0;
            while (conv_sample_cnt)
            {
                conversion_value += conv_samples[conv_sample_cnt];
                conv_sample_cnt--;
            }
            conversion_value /= ADS7841_OVERSAMPLE_COUNT;
        }
    }
    return conversion_value;
}


static void ADS7841_receive_byte_internal(uint8_t byte)
{
    static unsigned int byte_idx = 0;

    unsigned int max_byte_idx;
    max_byte_idx = sizeof(ADS7841_sample_holder.bytes) /
                   sizeof(*ADS7841_sample_holder.bytes);

    ADS7841_sample_holder.bytes[byte_idx] = byte;

    /* Wrap around condition */
    if (++byte_idx > max_byte_idx)
    {
        byte_idx = 0;
        if (conv_sample_cnt < ADS7841_OVERSAMPLE_COUNT)
        {
            ADS7841_sample_holder.conv_val &= ADS7841_cfg.sample_mask;
            conv_samples[conv_sample_cnt] = ADS7841_sample_holder.conv_val;
            conv_sample_cnt++;
        }
    }
}

static void ADS7841_get_conv_SINGLE(ADS7841_CHANNEL_t  ch,
                                    ADS7841_CONVTYPE_t conv_type)
{
    uint8_t  ctrl_byte;
    uint16_t power_mode = ADS7841_cfg.power_mode;
    ctrl_byte           = ADS7841_ctrl_byte(ch, power_mode, conv_type);

    uint8_t nul = '\0';

    SPI0_transmit(&ctrl_byte, 1);

    volatile int i;
    for (i = 0; i < 10000; i++)
    {
        /* wait for IC to process command */
    }

    SPI0_transmit(&nul, 1);

    for (i = 0; i < 10000; i++)
    {
        /* wait for IC to process command */
    }

    SPI0_transmit(&nul, 1);
}

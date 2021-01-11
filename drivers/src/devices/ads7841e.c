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

#define ADS7841_BITMASK12 (0x0FFF)
#define ADS7841_BITMASK8 (0x00FF)


typedef enum
{
    ADS7841_PWRMODE_inter_conv,
    ADS7841_PWRMODE_always_on,
} ADS7841_PWRMODE_t;


static buffer_handle ADS_rx_buf_handle;


static uint8_t ADS7841_ctrl_byte(ADS7841_CHANNEL_t  channel,
                                 ADS7841_PWRMODE_t  mode,
                                 ADS7841_CONVTYPE_t conv_type);

static void ADS7841_receive_byte_internal(uint8_t byte);


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


void ADS7841_driver_init(void)
{
    ADS_rx_buf_handle = bufferlib_ringbuf_new(4);
    SPI0_init(ADS7841_receive_byte_internal, SPI_DIR_msb, SPI_MODE_async);
}

void ADS7841_driver_deinit(void)
{
    ADS_rx_buf_handle.delete(ADS_rx_buf_handle.this);
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


uint16_t ADS7841_get_conv(ADS7841_CHANNEL_t ch, ADS7841_CONVTYPE_t conv_mode)
{
    uint8_t ctrl_byte;
    ctrl_byte = ADS7841_ctrl_byte(ch, ADS7841_PWRMODE_inter_conv, conv_mode);
    uint8_t msg[] = {ctrl_byte, 0, 0};
    SPI0_transmit(msg, sizeof(msg));

    uint8_t lo_byte;
    uint8_t hi_byte;
    lo_byte        = ADS_rx_buf_handle.read_next(ADS_rx_buf_handle.this);
    hi_byte        = ADS_rx_buf_handle.read_next(ADS_rx_buf_handle.this);
    uint16_t value = (hi_byte << CHAR_BIT) | lo_byte;
    switch (conv_mode)
    {
        case ADS7841_CONVTYPE_8:
        {
            value &= ADS7841_BITMASK8;
        }
        break;
        case ADS7841_CONVTYPE_12:
        {
            value &= ADS7841_BITMASK12;
        }
        break;
        default:
        {
        }
        break;
    }
    return value;
}


static void ADS7841_receive_byte_internal(uint8_t byte)
{
    ADS_rx_buf_handle.write_next(ADS_rx_buf_handle.this, byte);

    /** @todo DO SOME SORT OF CHECK WHETHER THE DATA IS VALID SINCE
     * THE SPI INTERFACE WORKS 1 BYTE AT A TIME, WE NEED TO
     * MAINTAIN A STATE MACHINE IF WE'RE RECEIVING THE FIRST OR SECOND DATA
     * BYTE FROM ADS
     */
}
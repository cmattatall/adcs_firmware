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
 * MSP430                      ADS7841
 * 5V --------------------------- 5V
 * GND -------------------------- GND
 * P3.0 (UCB0 MOSI) ------------- MOSI
 * P3.1 (UCB0 MISO) ------------- MISO
 * P3.2 (UCB0CLK) --------------- CLK
 * P2.3 Caller configured ------- CS
 */

#if !defined(TARGET_MCU)
#error DRIVER COMPILATION SHOULD ONLY OCCUR ON CROSSCOMPILED TARGETS
#endif /* !defined(TARGET_MCU) */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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
#define ADS7841_OVERSAMPLE_COUNT 1
#endif /* #if defined(ADS7841_OVERSAMPLE_COUNT) */

static struct
{
    ADS7841_PWRMODE_t  power_mode;
    ADS7841_CONVMODE_t conv_mode;
} ADS7841_cfg;

/* Valid conversion value buffer */
static volatile uint16_t conv_cnt;
static volatile uint16_t conv_samples[ADS7841_OVERSAMPLE_COUNT];


/* State machine for SPI receive events */
static volatile enum {
    ADS7841_RX_EVT_ctrl,
    ADS7841_RX_EVT_hi,
    ADS7841_RX_EVT_lo,
    ADS7841_RX_EVT_complete,
} ADS7841_RX_EVT;


/* Chip select functions. Caller provides as part of driver init
 * because SPI CS line may be MUXed with other devices */
static void (*ADS7841_SPI_CHIP_SELECT_func)(void)   = NULL;
static void (*ADS7841_SPI_CHIP_UNSELECT_func)(void) = NULL;


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


static const uint16_t ADS7841_conv_mode_map[] = {
    [ADS7841_CONVMODE_8]  = (CTL_MODE_8BIT),
    [ADS7841_CONVMODE_12] = (CTL_MODE_12BIT),
};

static uint8_t ADS7841_ctrl_byte(ADS7841_CHANNEL_t  channel,
                                 ADS7841_PWRMODE_t  pwr_mode,
                                 ADS7841_CONVMODE_t conv_mode);

static void ADS7841_receive_byte(uint8_t byte);
static int  ADS7841_conv_SINGLE(ADS7841_CHANNEL_t ch, ADS7841_CONVMODE_t type);

static void ADS7841_inter_byte_delay(void);

void ADS7841_driver_init(void (*ena_func)(void), void (*dis_func)(void),
                         ADS7841_PWRMODE_t mode, ADS7841_CONVMODE_t conv_mode)
{
    ADS7841_cfg.power_mode = mode;
    ADS7841_cfg.conv_mode  = conv_mode;

    ADS7841_SPI_CHIP_SELECT_func   = ena_func;
    ADS7841_SPI_CHIP_UNSELECT_func = dis_func;

    SPI_init_struct init;
    init.role       = SPI_ROLE_master;
    init.phy_cfg    = SPI_PHY_3;
    init.data_dir   = SPI_DATA_DIR_msb;
    init.tim_mode   = SPI_TIM_MODE_async;
    init.edge_phase = SPI_DATA_CHANGE_edge1;
    init.polarity   = SPI_CLK_POLARITY_low;

    uint16_t ADS7841_prescaler = 0x0040;
    SPI0_init(ADS7841_receive_byte, &init, ADS7841_prescaler);
}


void ADS7841_driver_deinit(void)
{
    ADS7841_SPI_CHIP_SELECT_func   = NULL;
    ADS7841_SPI_CHIP_UNSELECT_func = NULL;
    SPI0_deinit();
}


uint16_t ADS7841_measure_channel(ADS7841_CHANNEL_t ch)
{
    uint16_t conversion_value = ADS7841_CONV_STATUS_BUSY;
    CONFIG_ASSERT(ADS7841_SPI_CHIP_SELECT_func != NULL);
    CONFIG_ASSERT(ADS7841_SPI_CHIP_UNSELECT_func != NULL);
    memset((void *)conv_samples, 0, sizeof(conv_samples));
    ADS7841_SPI_CHIP_SELECT_func();

    volatile unsigned int conv_timeout;
    while (conv_cnt != ADS7841_OVERSAMPLE_COUNT)
    {
        /* Perform the required number of samples */
        ADS7841_conv_SINGLE(ch, ADS7841_cfg.conv_mode);

        conv_timeout = 0;
        while (ADS7841_RX_EVT != ADS7841_RX_EVT_complete)
        {
            /* wait for conversion to complete (with timeout) */
            if (++conv_timeout > 1000)
            {
                break;
            }
        }
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
    return conversion_value;
}


static void ADS7841_receive_byte(uint8_t byte)
{
    /* Temporary conversion value holder */
    static volatile union
    {
        uint8_t  bytes[2];
        uint16_t val;
    } conv_val_holder;

    switch (ADS7841_RX_EVT)
    {
        case ADS7841_RX_EVT_ctrl:
        {
            conv_val_holder.val = 0;

            /* Expect high byte next */
            ADS7841_RX_EVT = ADS7841_RX_EVT_hi;
        }
        break;
        case ADS7841_RX_EVT_hi:
        {
            if (ADS7841_cfg.conv_mode == ADS7841_CONVMODE_12)
            {
                /* Store high byte */
                conv_val_holder.bytes[1] = byte;

                /* Expect low byte next */
                ADS7841_RX_EVT = ADS7841_RX_EVT_lo;
            }
            else
            {
                conv_val_holder.bytes[0] = byte;

                /* If we don't have enough conversions,
                 * store the converted value */
                if (conv_cnt < ADS7841_OVERSAMPLE_COUNT)
                {
                    conv_samples[conv_cnt] = conv_val_holder.val;
                    conv_cnt++;
                }
                SPI0_disable_rx_irq();
                ADS7841_RX_EVT = ADS7841_RX_EVT_complete;
            }
        }
        break;
        case ADS7841_RX_EVT_lo:
        {
            conv_val_holder.bytes[0] = byte;

            /* If we don't have enough conversions, store the converted value */
            if (conv_cnt < ADS7841_OVERSAMPLE_COUNT)
            {
                /* truncate 16 bit to 12 */
                conv_samples[conv_cnt] = conv_val_holder.val >> 4;
                conv_cnt++;
            }
            SPI0_disable_rx_irq();
            ADS7841_RX_EVT = ADS7841_RX_EVT_complete;
        }
        break;
        default:
        {
            /* Do nothing */
        }
        break;
    }
}


static int ADS7841_conv_SINGLE(ADS7841_CHANNEL_t ch, ADS7841_CONVMODE_t type)
{
    uint16_t power_mode = ADS7841_cfg.power_mode;
    uint8_t  ctrl_byte  = ADS7841_ctrl_byte(ch, power_mode, type);
    uint8_t  msg[]      = {ctrl_byte, '\0', '\0'};
    uint16_t msglen     = (uint16_t)(sizeof(msg) / sizeof(*msg));
    ADS7841_RX_EVT      = ADS7841_RX_EVT_ctrl;
    SPI0_enable_rx_irq();
    int status = SPI0_transmit(msg, msglen, ADS7841_inter_byte_delay);
    return status;
}


static uint8_t ADS7841_ctrl_byte(ADS7841_CHANNEL_t  channel,
                                 ADS7841_PWRMODE_t  pwr_mode,
                                 ADS7841_CONVMODE_t conv_mode)
{
    uint8_t control_byte = 0;

    control_byte |= ADS7841_CHANNEL_MAP[channel];
    control_byte |= ADS7841_PWRMODE_MAP[pwr_mode];

    /* We're only going to do single-ended conversions so just force
     * the control byte into single mode */
    control_byte |= CTL_SGL_CONVERSION_SINGLE_ENDED;

    control_byte |= ADS7841_conv_mode_map[conv_mode];
    control_byte |= ((1 << CTL_START_POS) & CTL_START_MSK);
    return control_byte;
}


static void ADS7841_inter_byte_delay(void)
{
    /* So normally you're supposed to wait for the ADS7841 to pulse
     * its BUSY pin. But on the current implementation of sun sensor,
     * we do not have the BUSY pin connected to the MCU. Thus, I have to
     * introduce an artificial blocking delay between each 8 clock
     * pulses we send to the ADS7841 */
    volatile unsigned int i;
    for (i = 0; i < 50; i++)
    {
        /* Force delay */
    }
}

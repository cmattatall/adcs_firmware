#ifndef __SPI_H__
#define __SPI_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#if defined(TARGET_MCU)

/* UNCOMMENT THIS IF TO CATCH OVERRUNS */
/* #define SPI0_CATCH_OVERRUN */

#include <stdint.h>

#include "injection_api.h"


typedef enum
{
    SPI_PHY_3, /* 3 pin SPI */
    SPI_PHY_4, /* 4 pin SPI */
} SPI_PHY_t;

typedef enum
{
    SPI_ROLE_master,
    SPI_ROLE_slave,
} SPI_ROLE_t;

typedef enum
{
    SPI_DATA_DIR_lsb,
    SPI_DATA_DIR_msb,
} SPI_DATA_DIR_t;

typedef enum
{
    SPI_DATA_CHANGE_edge1, /* Data changed on edge1, captured on edge 2 */
    SPI_DATA_CHANGE_edge2, /* Data captured on edge1, changed on edge 2 */
} SPI_DATA_CHANGE_t;


typedef enum
{
    SPI_CLK_POLARITY_low,
    SPI_CLK_POLARITY_high,
} SPI_CLK_POLARITY_t;


typedef struct
{
    SPI_ROLE_t         role;
    SPI_PHY_t          phy_cfg;
    SPI_DATA_DIR_t     data_dir;
    SPI_DATA_CHANGE_t  edge_phase;
    SPI_CLK_POLARITY_t polarity;
} SPI_init_struct;

/**
 * @brief
 *
 * @param rx SPI receive event callback func
 * @param init init struct with init params
 * @param scaler clock prescaler
 */
void SPI0_init(receive_func rx, const SPI_init_struct *init, uint16_t scaler);


/**
 * @brief Deinitialize the SPI peripheral on UCB0
 */
void SPI0_deinit(void);


/**
 * @brief Transmit len bytes starting from bytes. After each byte is
 * transmitted, a callback function can be provided for execution.
 * @param bytes start of buffer to be transmitted
 * @param len number of bytes to be transmitted
 * @param tx_cb (optional) callback function to execute between transmits
 * @return int 0 if byte was transmitted successfully. Nonzero is SPI PHY is
 * busy.
 */
int SPI0_transmit(const uint8_t *bytes, uint16_t len);

/**
 * @brief Enable the receive event IRQ trigger for SPI0 on UCB0
 */
void SPI0_enable_rx_irq(void);


/**
 * @brief Disable the receive event IRQ trigger for SPI0 on UCB0
 */
void SPI0_disable_rx_irq(void);


#else

#error USE OF MEMORY MAPPED PERIPHERALS ON A VIRTUAL MEMORY NATIVE MACHINE IS NOT PERMITTED

#endif /* #if defined(TARGET_MCU) */

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __SPI_H__ */

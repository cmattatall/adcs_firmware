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
    SPI_DATA_DIR_lsb,
    SPI_DATA_DIR_msb,
} SPI_DATA_DIR_t;

typedef enum
{
    SPI_MODE_sync,
    SPI_MODE_async,
} SPI_MODE_t;


/**
 * @brief Initialize the SPI peripheral on UCB0
 *
 * @param rx byte receive callback function
 * @param dir data transmit order. one of SPI_DATA_DIR_t
 * @param mode SPI mode. SPI_MODE_sync or SPI_MODE_async
 */
void SPI0_init(receive_func rx, SPI_DATA_DIR_t dir, SPI_MODE_t mode);


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
int SPI0_transmit(const uint8_t *bytes, uint16_t len, void (*tx_cb)(void));

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

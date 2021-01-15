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
    SPI_DIR_lsb,
    SPI_DIR_msb,
} SPI_DIR_t;

typedef enum
{
    SPI_MODE_sync,
    SPI_MODE_async,
} SPI_MODE_t;

typedef enum
{
    SPI_IRQ_rx,
    SPI_IRQ_tx,
} SPI_IRQ_t;

void SPI0_enable_interrupt(SPI_IRQ_t irq);
void SPI0_disable_interrupt(SPI_IRQ_t irq);

void SPI0_init(receive_func rx, SPI_DIR_t dir, SPI_MODE_t mode);
void SPI0_deinit(void);

int SPI0_transmit(uint8_t *bytes, uint16_t len);


#else

#error USE OF MEMORY MAPPED PERIPHERALS ON A VIRTUAL MEMORY NATIVE MACHINE IS NOT PERMITTED

#endif /* #if defined(TARGET_MCU) */

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __SPI_H__ */

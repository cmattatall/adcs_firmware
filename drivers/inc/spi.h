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

void SPI0_init(receive_func rx);
void SPI0_deinit(void);

int SPI0_transmit_IT(uint8_t *bytes, uint16_t len);

#else

#error USE OF MEMORY MAPPED PERIPHERALS ON A VIRTUAL MEMORY NATIVE MACHINE IS NOT PERMITTED

#endif /* #if defined(TARGET_MCU) */

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __SPI_H__ */

#ifndef __SPI_H__
#define __SPI_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#if defined(TARGET_MCU)

#include <stdint.h>


#define SPI_APPLICATION_BUFFER_SIZE 500
#define SPI_DELIM_CHAR '\0'
#define SPI_SIGNAL_SET 1
#define SPI_SIGNAL_CLR 0

void         SPI0_init(volatile int **receive_signal_watcher,
                       volatile int **transmit_signal_watcher);
int          SPI0_receive_payload(uint8_t *userbuf, uint16_t len);
unsigned int SPI0_transmit_IT(uint8_t *bytes, uint16_t len);

#else

#error USE OF MEMORY MAPPED PERIPHERALS ON A VIRTUAL MEMORY NATIVE MACHINE IS NOT PERMITTED

#endif /* #if defined(TARGET_MCU) */

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __SPI_H__ */

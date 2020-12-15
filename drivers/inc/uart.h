#ifndef __UART_H__
#define __UART_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */


#if defined(TARGET_MCU)

void uart_init(void (*receive_byte_func)(uint8_t));
void uart_deinit(void);
int  uart_transmit(uint8_t *buf, uint_least16_t buflen);

#else

#error USE OF MEMORY MAPPED PERIPHERALS ON A VIRTUAL MEMORY NATIVE MACHINE IS NOT PERMITTED

#endif /* #if defined(TARGET_MCU) */

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __UART_H__ */

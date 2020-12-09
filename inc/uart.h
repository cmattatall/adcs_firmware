#ifndef __UART_H__
#define __UART_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

void uart_init(void);
void uart_deinit(void);

int uart_transmit(uint8_t *buf, uint_least16_t buflen);
int uart_receive(uint8_t *buf, uint_least16_t buflen);

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __UART_H__ */

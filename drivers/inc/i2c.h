#ifndef __I2C_H__
#define __I2C_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#include <stdint.h>

void I2C0_init(void);
int I2C0_write_bytes(uint8_t dev_addr, uint8_t *bytes, uint16_t byte_count);
int  I2C0_read_bytes(uint8_t *caller_buf, uint16_t caller_buflen);


void I2C1_init(void);
int I2C1_write_bytes(uint8_t dev_addr, uint8_t *bytes, uint16_t byte_count);
int  I2C1_read_bytes(uint8_t *caller_buf, uint16_t caller_buflen);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __I2C_H__ */

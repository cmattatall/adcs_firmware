#ifndef __ADS7841E_H__
#define __ADS7841E_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#include <stdint.h>

typedef enum
{
    ADS7841_CHANNEL_0,
    ADS7841_CHANNEL_1,
    ADS7841_CHANNEL_2,
    ADS7841_CHANNEL_3,
} ADS7841_CHANNEL_t;

typedef enum
{
    ADS7841_CONVTYPE_8,
    ADS7841_CONVTYPE_12,
} ADS7841_CONVTYPE_t;

void ADS7841_driver_init(void);

void ADS7841_driver_deinit(void);

uint16_t ADS7841_get_conv(ADS7841_CHANNEL_t ch, ADS7841_CONVTYPE_t conv_mode);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __ADS7841E_H__ */

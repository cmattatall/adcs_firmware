#ifndef __ADS7841E_H__
#define __ADS7841E_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

#define ADS7841_CONV_STATUS_ERROR (UINT16_MAX)
#define ADS7841_CONV_STATUS_BUSY (UINT16_MAX - 1)

typedef enum
{
    ADS7841_CHANNEL_0,
    ADS7841_CHANNEL_1,
    ADS7841_CHANNEL_2,
    ADS7841_CHANNEL_3,
} ADS7841_CHANNEL_t;

typedef enum
{
    ADS7841_CONVMODE_8,
    ADS7841_CONVMODE_12,
} ADS7841_CONVMODE_t;

typedef enum
{
    ADS7841_PWRMODE_inter_conv,
    ADS7841_PWRMODE_always_on,
} ADS7841_PWRMODE_t;

void ADS7841_driver_init(void (*ena_func)(void), void (*dis_func)(void),
                         ADS7841_PWRMODE_t mode, ADS7841_CONVMODE_t conv_type);

void ADS7841_driver_deinit(void);

uint16_t ADS7841_measure_channel(ADS7841_CHANNEL_t ch);


#warning REMOVE ME LATER
void ADS7841_TEST(void);

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __ADS7841E_H__ */

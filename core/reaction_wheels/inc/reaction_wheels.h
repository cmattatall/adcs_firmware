#ifndef __REAC_WHEELS_H__
#define __REAC_WHEELS_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#include <stdint.h>
#include "pwm.h"
#include "rotational_directions.h"

#define NUM_REACTION_WHEELS ((unsigned int)(3))

typedef enum
{
    REAC_WHEEL_x,
    REAC_WHEEL_y,
    REAC_WHEEL_z,
} REAC_WHEEL_t;

void RW_init(void);

void RW_set_speed_rph(REAC_WHEEL_t rw, int rph); /* rph == radians per hour */

int RW_config_to_string(char *buf, int buflen);

int RW_measure_current_ma(REAC_WHEEL_t wheel);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __REAC_WHEELS_H__ */

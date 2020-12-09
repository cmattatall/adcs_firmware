#ifndef __REACTION_WHEELS_H__
#define __REACTION_WHEELS_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#include <stdint.h>
#include "pwm.h"

typedef enum
{
    REACTION_WHEEL_x,
    REACTION_WHEEL_y,
    REACTION_WHEEL_z,
} REACTION_WHEEL_t;

int set_reaction_wheel_pwm(REACTION_WHEEL_t wheel, pwm_t value);


pwm_t get_reaction_wheel_pwm(REACTION_WHEEL_t wheel);

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __REACTION_WHEELS_H__ */

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

typedef enum
{
    RW_DIR_clockwise,
    RW_DIR_anticlockwise,
    RW_DIR_invalid, /* <-- USED TO INDICATE API ERROR TO CALLER */
} RW_DIR_t;



void set_reaction_wheel_pwm(REACTION_WHEEL_t wheel, pwm_t value);
void set_reaction_wheel_dir(REACTION_WHEEL_t wheel, RW_DIR_t dir);


pwm_t    get_reaction_wheel_pwm(REACTION_WHEEL_t wheel);
RW_DIR_t get_reaction_wheel_dir(REACTION_WHEEL_t wheel);

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __REACTION_WHEELS_H__ */

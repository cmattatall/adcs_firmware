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

#define RW_PWM_DEFAULT ((pwm_t)(PWM_DEFAULT))

typedef enum
{
    REAC_WHEEL_DIR_pos = ROT_DIR_clock,
    REAC_WHEEL_DIR_neg = ROT_DIR_anticlock,
} REAC_WHEEL_DIR_t;

typedef struct
{
    pwm_t            pwm;
    REAC_WHEEL_DIR_t dir;
} reac_wheel_config_single;

typedef struct
{
    reac_wheel_config_single configs[NUM_REACTION_WHEELS];
} reac_wheel_configs;


reac_wheel_configs       rw_get_configs(void);
reac_wheel_config_single rw_get_config(REAC_WHEEL_t wheel);
void rw_set_config(REAC_WHEEL_t wheel, const reac_wheel_config_single *config);
void reac_wheel_apply_configs(void);
int reac_wheel_config_to_string(char *buf, unsigned int buflen,
                                const reac_wheel_configs *configs);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __REAC_WHEELS_H__ */

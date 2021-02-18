#ifndef __MAGNETORQUERS_H__
#define __MAGNETORQUERS_H__
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
    MQTR_x,
    MQTR_y,
    MQTR_z,
} MQTR_t;


typedef enum
{
    MQTR_DIR_clockwise,
    MQTR_DIR_anticlockwise,
    MQTR_DIR_invalid, /* <-- USED TO INDICATE API ERROR TO CALLER */
} MQTR_DIR_t;


#define MQTR_PWM_DEFAULT ((pwm_t)(PWM_DEFAULT))


pwm_t      mqtr_set_pwm(MQTR_t wheel, pwm_t value);
MQTR_DIR_t mqtr_set_dir(MQTR_t wheel, MQTR_DIR_t dir);


pwm_t      mqtr_get_pwm(MQTR_t wheel);
MQTR_DIR_t mqtr_get_dir(MQTR_t wheel);

char *mqtr_dir_str(MQTR_DIR_t dir);



void mqtr_config_apply(void);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __MAGNETORQUERS_H__ */

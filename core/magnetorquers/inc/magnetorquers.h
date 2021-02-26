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
#include "rotational_directions.h"


typedef enum
{
    MQTR_x,
    MQTR_y,
    MQTR_z,
} MQTR_t;

typedef enum
{
    MQTR_DIR_pos = ROT_DIR_clock,
    MQTR_DIR_neg = ROT_DIR_anticlock,
} MQTR_DIR_t;

void mqtr_config_update(MQTR_t mqtr, unsigned int voltage_mv, MQTR_DIR_t dir);

void mqtr_config_apply(void);

int mqtr_config_to_str(char *buf, unsigned int buflen);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __MAGNETORQUERS_H__ */

#ifndef __MAGNETORQUERS_H__
#define __MAGNETORQUERS_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */


#include <stdint.h>


typedef enum
{
    MQTR_x,
    MQTR_y,
    MQTR_z,
} MQTR_t;

void MQTR_init(void);

void MQTR_set_coil_voltage_mv(MQTR_t mqtr, int voltage_mv);

int MQTR_config_to_str(char *buf, int buflen);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __MAGNETORQUERS_H__ */

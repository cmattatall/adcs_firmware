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


/**
 * @brief set coil voltage on mqtr pwm pin
 * 
 * @param mqtr 
 * @param voltage_mv (positive == R coil pin, negative == F coil pin)
 * 
 * @note 
 * X : RERVERSE HBRIDGE == P1.2 TA0CCR1, FWD HBRIDGE == P1.3 TA0CCR2
 * Y : REVERSE HBRIDGE == P2.1, FWD HBRIDGE == P2.0
 * Z : REVERSE HBRIDGE == P1.5 TA0CCR4, FWD HBRIDGE == P1.4 TA0CCR3
 */
void MQTR_set_coil_voltage_mv(MQTR_t mqtr, int voltage_mv);

int MQTR_config_to_str(char *buf, int buflen);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __MAGNETORQUERS_H__ */

#ifndef __PWM_H__
#define __PWM_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#include <stdint.h>
#include <limits.h>


typedef uint16_t pwm_t;

#define PWM_DEFAULT (pwm_t)(0)

#define PWM_MIN ((pwm_t)(0))
#define PWM_MAX ((pwm_t)((UINT16_MAX)-1))
#define PWM_INVALID ((pwm_t)((PWM_MAX) + 1))

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __PWM_H__ */

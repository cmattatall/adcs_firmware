#ifndef __TIMERS_H__
#define __TIMERS_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */
#include <stdint.h>
#include <stdbool.h>

void TIMERA0_init(void);

extern volatile bool heartbeat_flag;

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __TIMERS_H__ */

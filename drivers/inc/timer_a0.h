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

#include "callback_api.h"

void TIMERA0_heartbeat_init(void);
void TIMERA0_register_callback(callback_handle_t cb);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __TIMERS_H__ */

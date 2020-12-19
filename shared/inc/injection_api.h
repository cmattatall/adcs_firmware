#ifndef __INJECTION_API_H__
#define __INJECTION_API_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#define UNREGISTERED_CALLBACK_HANDLE NULL

#include <stdint.h>

#include "callback.h"

typedef void(*cb_injector_func(callback_func));

typedef int (*transmit_func)(uint8_t *, uint16_t);
typedef void (*tx_injector_func)(transmit_func);

typedef void (*receive_func)(uint8_t);
typedef void (*rx_injector_func)(receive_func);

typedef void (*init_func)(void);
typedef void (*init_injector_func)(init_func);

typedef void (*deinit_func)(void);
typedef void (*deinit_injector_func)(deinit_func);

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __INJECTION_API_H__ */

#ifndef __BSP_H__
#define __BSP_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */
#include "callback_api.h"

void BSP_init(void);
void BSP_toggle_red_led(__unused callback_args args);
void BSP_set_red_led(__unused callback_args args);
void BSP_clr_red_led(__unused callback_args args);

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __BSP_H__ */

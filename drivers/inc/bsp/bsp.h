#ifndef __BSP_H__
#define __BSP_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */
#include "injection_api.h"

/**
 * @brief Initialize the board support function stuff for the launchpad
 *
 */
void BSP_init(void);

/**
 * @brief Toggle onboard red LED
 *
 * @param args UNUSED
 */
void BSP_toggle_red_led(callback_args args);

/**
 * @brief turn on the onboard red LED
 *
 * @param args UNUSED
 */
void BSP_set_red_led(callback_args args);

/**
 * @brief turn off the onboard red LED
 *
 * @param args UNUSED
 */
void BSP_clr_red_led(callback_args args);

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __BSP_H__ */

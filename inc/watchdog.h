#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

void watchdog_stop(void);
void watchdog_start(void);
void watchdog_kick(void);

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __WATCHDOG_H__ */

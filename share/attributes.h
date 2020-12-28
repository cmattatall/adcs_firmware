#ifndef __ATTRIBUTES_H__
#define __ATTRIBUTES_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

/* clang-format off */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#define __TEST_HOOKABLE __weak
#elif defined(__GNUC__)
#define __irq_default(isr_code) __attribute__((weak, interrupt(isr_code)))
#define __TEST_HOOKABLE __attribute__((weak))
#else
#error Compiler not supported!
#endif

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __ATTRIBUTES_H__ */

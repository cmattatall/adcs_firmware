#ifndef __ATTRIBUTES_H__
#define __ATTRIBUTES_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

/* If you like preprocessor abuse, you're gonna love this file... */

/* clang-format off */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#ifdef __interrupt
#undef __interrupt
#endif
#define __interrupt(isr) _Pragma vector = isr __interrupt
#elif defined(__GNUC__) || defined(__GNUG__)
#define __weak __attribute__((weak))
#endif

#ifndef __EMULATABLE
#define __EMULATABLE __weak
#endif /* ifndef __EMULATABLE */

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __ATTRIBUTES_H__ */

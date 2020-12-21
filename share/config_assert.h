#ifndef __CONFIG_ASSERT_H__
#define __CONFIG_ASSERT_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

/* CONFIG_ASSERT */
#if defined(TARGET_MCU)
#if defined(DEBUG) && !defined(NDEBUG)
#define CONFIG_ASSERT(x)                                                       \
    do                                                                         \
    {                                                                          \
        if (!(x))                                                              \
        {                                                                      \
            while (1)                                                          \
            {                                                                  \
            }                                                                  \
        }                                                                      \
    } while (0)
#else
#define CONFIG_ASSERT(x) ; /* literally let compiler elide the check */
#endif                     /* #if defined(DEBUG)  && !defined(NDEBUG) */
#else                      /* TARGET_MCU NOT DEFINED */
#if defined(DEBUG) && !defined(NDEBUG)
#include <assert.h>
#define CONFIG_ASSERT(x) assert((x))
#else
/* CAST AS VOID TO PREVENT COMPILER WARNING OF UNUSED VAR */
#define CONFIG_ASSERT(x) (void)(x);
#endif /* #if defined(DEBUG)  && !defined(NDEBUG) */

#endif /* #if defined(TARGET_MCU) */

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __CONFIG_ASSERT_H__ */

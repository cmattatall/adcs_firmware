#ifndef __PLATFORM_H__
#define __PLATFORM_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#if defined(TARGET_MCU)
#include <msp430.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
// define something for Windows (32-bit and 64-bit, this part is common)
#include <windows.h>
#ifdef _WIN64
// define something for Windows (64-bit only)
#else
// define something for Windows (32-bit only)
#endif
#elif __APPLE__
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#elif TARGET_OS_IPHONE
// iOS device
#elif TARGET_OS_MAC
// Other kinds of Mac OS
#else
#error "Unknown Apple platform"
#endif
#elif __linux__
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#elif __unix__ // all unices not caught above
// Unix
#elif defined(_POSIX_VERSION)
// POSIX
#else
#error "Unknown compiler"
#endif
#endif /* #if defined(TARGET_MCU) */

/* COMPILER ATTRIBUTES */
/* clang-format off */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#define weak __weak
#define packed __packed
#elif defined(__GNUC__)
#define weak __attribute__((weak))
#define packed __attribute__((packed))
#else
#error Compiler not supported!
#endif

/* log_trace */
#if defined(TARGET_MCU)
#define log_trace(fmt, ...) ; /* do nothing (this is just so it compiles) */
#else
#define log_trace(fmt, ...) printf("[%s : %d in %s] :" fmt, __FILE__, __LINE__, __func__, ## __VA_ARGS__)
#endif /* #if defined(TARGET_MCU) */

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
#define CONFIG_ASSERT(x) ; /* literally let compiler elide the check */
#endif                     /* #if defined(DEBUG)  && !defined(NDEBUG) */

#endif /* #if defined(TARGET_MCU) */


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __PLATFORM_H__ */

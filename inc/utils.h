#ifndef __UTILS_H__
#define __UTILS_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */


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


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __UTILS_H__ */

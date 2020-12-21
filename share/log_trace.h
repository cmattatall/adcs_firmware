#ifndef __LOG_TRACE_H__
#define __LOG_TRACE_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

/* log_trace */
#if defined(TARGET_MCU)
#define log_trace(fmt, ...) ; /* do nothing (this is just so it compiles) */
#else
#define log_trace(fmt, ...)                                                    \
    printf("[%s : %d in %s]\n>>> " fmt "\n", __FILE__, __LINE__, __func__,     \
           ##__VA_ARGS__)
#endif /* #if defined(TARGET_MCU) */

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __LOG_TRACE_H__ */

#ifndef __OBC_EMULATOR_H__
#define __OBC_EMULATOR_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#if defined(TARGET_MCU)
#error EMULATION OF HARDWARE IS INTENDED FOR TESTING ON NATIVE PLATFORMS
#else

#endif /* #if defined(TARGET_MCU) */

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __OBC_EMULATOR_H__ */

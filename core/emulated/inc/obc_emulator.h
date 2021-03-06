#ifndef __OBC_EMULATOR_H__
#define __OBC_EMULATOR_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */
#if !defined(TARGET_MCU)

/**
 * @brief start the UART emulator to the OBC as an emulated hardware interface
 *
 * @note THIS IS INTENDED TO BE USED WHEN TESTING APPLICATION LOGIC ON A
 *       HOST MACHINE (rather than the target MCU)
 */
void OBC_EMU_start(void);

/**
 * @brief TX wrapper forOBC emulator to allow dependency injection into OBC IF
 *
 * @param buf buffer to transmit
 * @param buflen # of bytes to transmit
 * @return number of bytes transmitted (just like printf)
 */
int OBC_EMU_tx(uint8_t *buf, uint_least16_t buflen);

#else
#error EMULATION OF HARDWARE IS INTENDED FOR TESTING ON NATIVE PLATFORMS
#endif /* !#if defined(TARGET_MCU) */

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __OBC_EMULATOR_H__ */

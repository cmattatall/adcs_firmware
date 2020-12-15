#ifndef __OBC_INTERFACE_H__
#define __OBC_INTERFACE_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */


#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

#if !defined(TARGET_MCU)
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#endif /* #if !defined(TARGET_MCU) */


#define OBC_MSG_DELIM '!' /* for now we can just use ! */

#define OBC_IF_DATA_RX_FLAG_SET true
#define OBC_IF_DATA_RX_FLAG_CLR false

#define OBC_TX_BUFFER_SIZE 500
extern uint8_t txBufIdx;
extern uint8_t obcTxBuf[2][OBC_TX_BUFFER_SIZE];

/**
 * @brief Configure OBC interface for communication via dependency injection
 *
 * @param init peripheral initialization function. Accepts
 * @param deinit peripheral deinitialization function
 * @param tx transmit function.
 * @return int return code. returns 0 upon success. non-zero indicates error.
 */
int OBC_IF_config(void (*init)(void (*rx_func)(uint8_t)), void (*deinit)(void),
                  int (*tx)(uint8_t *, uint_least16_t));

/**
 * @brief Clear OBC interface driver configuration
 */
void OBC_IF_clear_config(void);

/**
 * @brief Transmit bytes to OBC
 * @param buf buffer to transmit
 * @param buflen max lenght of buffer to transmit
 * @return int number of bytes transmitted successfully
 */
int OBC_IF_tx(uint8_t *buf, uint_least16_t buflen);


#if !defined(TARGET_MCU)
/**
 * @brief receive a byte into the OBC communication interface
 *
 * @param byte byte to receive
 */
void OBC_IF_receive_byte(uint8_t byte);
#endif /* #if defined(TARGET_MCU) */

/**
 * @brief
 *
 * @param buf
 * @param buflen
 * @return int
 */
int OCB_IF_get_command_string(uint8_t *buf, uint_least16_t buflen);


/**
 * @brief thread-safe read for OBC_IF data received flag
 *
 * @return true if flag is true
 * @return false if flag is false
 */
bool OBC_IF_dataRxFlag_read(void);

/**
 * @brief thread-safe write to OBC data received flag
 *
 * @param data_state the state to set the data received flag
 */
void OBC_IF_dataRxFlag_write(bool data_state);


/* Printf wrapper for transmit to make life a bit easier */
#define __OBC_tx_wrapper(fmt, ...)                                             \
    do                                                                         \
    {                                                                          \
        memset(obcTxBuf[txBufIdx], 0, sizeof(obcTxBuf[txBufIdx]));             \
        snprintf((char *)obcTxBuf[txBufIdx], sizeof(obcTxBuf[txBufIdx]), fmt,  \
                 ##__VA_ARGS__);                                               \
        if (++txBufIdx > 1)                                                    \
        {                                                                      \
            txBufIdx = 0;                                                      \
        }                                                                      \
        OBC_IF_tx(obcTxBuf[txBufIdx], sizeof(obcTxBuf[txBufIdx]));             \
    } while (0)

#define OBC_printf(fmt, ...)                                                   \
    __OBC_tx_wrapper(fmt "%c", ##__VA_ARGS__, OBC_MSG_DELIM)

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __OBC_INTERFACE_H__ */

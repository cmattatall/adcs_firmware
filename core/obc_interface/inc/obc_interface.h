#ifndef __OBC_INTERFACE_H__
#define __OBC_INTERFACE_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */


typedef enum
{
    OBC_IF_PHY_CFG_UART,
    OBC_IF_PHY_CFG_EMULATED,
} OBC_IF_PHY_CFG_t;

#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

#if !defined(TARGET_MCU)
#include <pthread.h>
#endif /* #if !defined(TARGET_MCU) */


#ifndef OBC_MSG_DELIM

#define OBC_MSG_DELIM ((char)'!') /* for now we can just use ! */

#endif /* ifndef OBC_MSG_DELIM */


#define OBC_IF_DATA_RX_FLAG_SET true
#define OBC_IF_DATA_RX_FLAG_CLR false

#define OBC_TX_BUFFER_SIZE 500

/**
 * @brief Configure the OBC Communication interface
 *
 * @param config_mode one of OBC_IF_PHY_CONFIG_t
 * @return int 0 on success, !0 on error
 */
int OBC_IF_config(OBC_IF_PHY_CFG_t config_mode);

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


/**
 * @brief Printf wrapper for OBC_IF_tx to make life easier
 *
 * @param fmt printf style format string
 * @param ... __VA_ARGS__
 * @return int number of bytes transmitted
 */
int OBC_IF_printf(const char *restrict fmt, ...);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __OBC_INTERFACE_H__ */

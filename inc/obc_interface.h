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

#define OBC_INTERFACE_RECEIVE_DELIMITER '!' /* for now we can just use ! */

extern volatile bool OBC_IF_data_received_flag;

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


/**
 * @brief
 *
 * @param buf
 * @param buflen
 * @return int
 */
int OCB_IF_get_command_string(uint8_t *buf, uint_least16_t buflen);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __OBC_INTERFACE_H__ */

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

extern volatile bool OBC_if_data_received_flag;

/**
 * @brief Configure OBC interface for communication via dependency injection
 *
 * @param init peripheral initialization function
 * @param deinit peripheral deinitialization function
 * @param rx receive function. CALLED AUTOMATICALLY FROM ISR CONTEXT
 * @param tx transmit function.
 * @return int return code. returns 0 upon success. non-zero indicates error.
 *
 * @note rx must be reentrant
 */
int OBC_if_config(void (*init)(void), void (*deinit)(void),
                  int (*rx)(uint8_t *, uint_least16_t),
                  int (*tx)(uint8_t *, uint_least16_t));

/**
 * @brief Clear
 *
 */
void OBC_if_clear_config(void);


int OBC_if_tx(uint8_t *buf, uint_least16_t buflen);

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __OBC_INTERFACE_H__ */

/**
 * @file uart_interface.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Abstraction layer between PHY peripheral interface and
 * application level protocol
 * @version 0.1
 * @date 2020-12-09
 *
 * @copyright Copyright (c) 2020 DSS - LORIS project
 *
 * @note
 * @todo
 */
#include <stdio.h> /* snprintf */
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include "targets.h"

#include "obc_interface.h"

#include "bufferlib.h"

#if !defined(TARGET_MCU)

#include <pthread.h>
static pthread_mutex_t OBC_IF_rxflag_lock;
#endif /* !defined(TARGET_MCU) */

#define OBC_INTERFACE_BUFFER_SIZE 2000

typedef struct
{
    rx_injector_func init;
    deinit_func      deinit;
    transmit_func    tx;
} OBC_IF_fops;


/**
 * @brief Receive callback to be injected from OBC IF into LL driver
 *
 * @param byte byte to receive from driver
 */
static void OBC_IF_receive_byte_internal(uint8_t byte);


static OBC_IF_fops   ops           = {NULL};
static volatile bool OBC_IF_rxflag = false;
static ringbuf_t     OBC_IF_rxbuf_handle_internal;
static uint8_t       obcTxBuf[OBC_INTERFACE_BUFFER_SIZE];


int OBC_IF_config(rx_injector_func init, deinit_func deinit, transmit_func tx)
{
    int status = 0;
    ops.init   = init;
    ops.deinit = deinit;
    ops.tx     = tx;

    OBC_IF_rxbuf_handle_internal = ringbuf_ctor(OBC_INTERFACE_BUFFER_SIZE);

#if !defined(TARGET_MCU)
    pthread_mutex_init(&OBC_IF_rxflag_lock, NULL);
#endif /* !defined(TARGET_MCU) */

    if (ops.init != NULL)
    {
        ops.init(OBC_IF_receive_byte_internal);
    }

    if (ops.tx == NULL)
    {
        status = 1;
    }


    return status;
}


void OBC_IF_clear_config(void)
{
    ops.init = NULL;
    ops.tx   = NULL;
    if (ops.deinit != NULL)
    {
        ops.deinit();
        ops.deinit = NULL;
    }

    ringbuf_dtor(OBC_IF_rxbuf_handle_internal);

#if !defined(TARGET_MCU)
    pthread_mutex_destroy(&OBC_IF_rxflag_lock);
#endif /* !defined(TARGET_MCU) */
}


#if !defined(TARGET_MCU)
void OBC_IF_receive_byte(uint8_t byte)
{
    OBC_IF_receive_byte_internal(byte);
}
#endif /* #if defined(TARGET_MCU) */

int OCB_IF_get_command_string(uint8_t *buf, uint_least16_t buflen)
{
    CONFIG_ASSERT(buf != NULL);
    int           status      = 0;
    bool          found_delim = false;
    uint_fast16_t i           = 0;
    do
    {
        char *tmp = ringbuf_read_next(OBC_IF_rxbuf_handle_internal);
        if (tmp != NULL)
        {
            buf[i] = *tmp;
            if (buf[i] == OBC_MSG_DELIM)
            {
                buf[i]      = '\0';
                found_delim = true;
            }
        }
        else
        {
            break;
        }
    } while (!found_delim && ++i < buflen);

    if (!found_delim)
    {
        status = 1;
    }
    return status;
}


__attribute__((weak)) int OBC_IF_tx(uint8_t *buf, uint_least16_t buflen)
{
    CONFIG_ASSERT(ops.tx != NULL);
    return ops.tx(buf, buflen);
}


bool OBC_IF_dataRxFlag_read(void)
{
    bool flag_state;

#if !defined(TARGET_MCU)
    pthread_mutex_lock(&OBC_IF_rxflag_lock);
#endif /* #if defined(TARGET_MCU) */

    flag_state = OBC_IF_rxflag;

#if !defined(TARGET_MCU)
    pthread_mutex_unlock(&OBC_IF_rxflag_lock);
#endif /* #if defined(TARGET_MCU) */

    return flag_state;
}

void OBC_IF_dataRxFlag_write(bool data_state)
{
#if !defined(TARGET_MCU)
    pthread_mutex_lock(&OBC_IF_rxflag_lock);
#endif /* #if defined(TARGET_MCU) */

    OBC_IF_rxflag = data_state;

#if !defined(TARGET_MCU)
    pthread_mutex_unlock(&OBC_IF_rxflag_lock);
#endif /* #if defined(TARGET_MCU) */
}


static void OBC_IF_receive_byte_internal(uint8_t byte)
{
    ringbuf_write_next_byte(OBC_IF_rxbuf_handle_internal, byte);
    if (byte == OBC_MSG_DELIM)
    {
        OBC_IF_dataRxFlag_write(OBC_IF_DATA_RX_FLAG_SET);
    }
}


/** @todo THIS FUNCTION IS SO GODDAMN UGLY BUT AT LEAST ITS WORKING - Carl */
int OBC_IF_printf(const char *restrict fmt, ...)
{
    CONFIG_ASSERT(fmt != NULL);
    int     bytes_transmitted = 0;
    va_list args;
    va_start(args, fmt);

#if defined(TARGET_MCU)
    char *fmt_str = (char *)fmt;
#else
    /** @note why the fuck do I even have to add this. I shouldn't have to add
     * it. In fact, according to POSIX spec for termios I shouldn't even have
     * to WORRY about it...
     *
     * The entire reason this is here is so that the god damn newline gets
     * appended to the format string because aparently, POSIX shells can't
     * fucking detect EOL signals consistently...
     *
     * I've spend HOURS finding the problem and then fixing it.
     *
     * IF YOU TOUCH THIS PART I WILL ROLL OVER IN MY GRAVE...
     *
     * - Carl
     */
    char fmt_str[250] = {'\0'};
    CONFIG_ASSERT(strnlen(fmt, sizeof(fmt_str)) <= sizeof(fmt_str));
    strcpy(fmt_str, fmt);
    strcat(fmt_str, "\n");
#endif /* #if defined(TARGET_MCU) */

    memset(obcTxBuf, 0, sizeof(obcTxBuf));
    vsnprintf((char *)obcTxBuf, sizeof(obcTxBuf), fmt_str, args);
    size_t msg_len = strnlen((char *)obcTxBuf, sizeof(obcTxBuf));

    bytes_transmitted = OBC_IF_tx(obcTxBuf, msg_len);
    va_end(args);
    return bytes_transmitted;
}

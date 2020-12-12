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
#include <stdint.h>
#include <limits.h>
#include "platform.h"

#include "obc_interface.h"

uint8_t txBufIdx;
uint8_t obcTxBuf[2][OBC_TX_BUFFER_SIZE];

typedef struct
{
    void (*init)(void (*rx_func)(uint8_t));
    void (*deinit)(void);
    int (*tx)(uint8_t *, uint_least16_t);
} OBC_IF_fops;

/**
 * @brief CALLED FROM ISR CONTEXT
 *
 * @param byte byte to plce into next spot of buffer (received from OBC)
 */
static void OBC_IF_receive_byte(uint8_t byte);

static uint8_t  ringbuf[2000];
static uint8_t *inPtr;  /* Read in pointer */
static uint8_t *outPtr; /* Read out pointer */

static OBC_IF_fops ops = {NULL};

volatile bool OBC_IF_data_received_flag = false;


int OBC_IF_config(void (*init)(void (*rx_func)(uint8_t)), void (*deinit)(void),
                  int (*tx)(uint8_t *, uint_least16_t))
{
    int status = 0;
    ops.init   = init;
    ops.deinit = deinit;
    ops.tx     = tx;

    CONFIG_ASSERT(ops.init != NULL);
    ops.init(OBC_IF_receive_byte);

    if (tx == NULL)
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
}


static void OBC_IF_receive_byte(uint8_t byte)
{
    *inPtr = byte;
    if (*inPtr == OBC_MSG_DELIM)
    {
        OBC_IF_data_received_flag = true;
    }

    if (inPtr = &ringbuf[sizeof(ringbuf) - 1])
    {
        inPtr = ringbuf;
    }
    else
    {
        inPtr++;
    }
}


int OCB_IF_get_command_string(uint8_t *buf, uint_least16_t buflen)
{
    CONFIG_ASSERT(buf != NULL);
    int           status      = 0;
    bool          found_delim = false;
    uint_fast16_t i           = 0;
    do
    {
        buf[i] = *outPtr;
        if (buf[i] == OBC_MSG_DELIM)
        {
            found_delim = true;
        }

        if (outPtr = &ringbuf[sizeof(ringbuf) - 1])
        {
            outPtr = ringbuf;
        }
        else
        {
            outPtr++;
        }
    } while (!found_delim && ++i < buflen);

    if (!found_delim)
    {
        status = 1;
    }
    return status;
}


int OBC_IF_tx(uint8_t *buf, uint_least16_t buflen)
{
    ops.tx(buf, buflen);
}

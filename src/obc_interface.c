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

#include "obc_interface.h"

#include <string.h>

typedef struct
{
    void (*init)(void);
    void (*deinit)(void);
    int (*rx)(uint8_t *, uint_least16_t);
    int (*tx)(uint8_t *, uint_least16_t);
} OBC_if_fops;

static OBC_if_fops ops = {NULL};

volatile bool OBC_if_data_received_flag = false;

static uint8_t  ringbuf[2000];
static uint8_t *inPtr;  /* Read in pointer */
static uint8_t *outPtr; /* Read out pointer */


int OBC_if_config(void (*init)(void), void (*deinit)(void),
                  int (*rx)(uint8_t *, uint_least16_t),
                  int (*tx)(uint8_t *, uint_least16_t))
{
    int status = 0;
    ops.init   = init;
    ops.deinit = deinit;
    ops.rx     = rx;
    ops.tx     = tx;

    inPtr  = ringbuf;
    outPtr = ringbuf;

    if (ops.init != NULL)
    {
        ops.init();
    }

    if (rx == NULL || tx == NULL)
    {
        status = 1;
    }

    return status;
}


void OBC_if_clear_config(void)
{
    ops.init = NULL;
    ops.rx   = NULL;
    ops.tx   = NULL;
    if (ops.deinit != NULL)
    {
        ops.deinit;
    }
    ops.deinit = NULL;
    inPtr      = ringbuf;
    outPtr     = ringbuf;
    memset(ringbuf, '\0', sizeof(ringbuf));
}

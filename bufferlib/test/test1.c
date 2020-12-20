/**
 * @file test1.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief
 * @version 0.1
 * @date 2020-12-20
 *
 * @copyright Copyright (c) 2020 carl mattatall
 */
#include <stdio.h>
#include <assert.h>

#include "ringbuf.h"

int main(void)
{
    ringbuf_t buf = ringbuf_ctor(2000);
    assert(buf != NULL);


    ringbuf_dtor(buf);

    return 0;
}
/**
 * @file json_test_hooks.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief source module to produce test hooks for ADCS json parsing tests
 * @version 0.1
 * @date 2020-12-22
 * 
 * @copyright Copyright (c) 2020 carl mattatall
 * 
 * @note
 * @todo
 */
#include <stdio.h>
#include <string.h>

#include "json_test_hooks.h"

#include "attributes.h"

uint8_t OBC_MESSAGE_SIPHON_BUFFER[250];


int OBC_IF_tx(uint8_t *buf, uint_least16_t buflen)
{
    printf("%.*s\n", buflen, buf);
    return buflen;
}

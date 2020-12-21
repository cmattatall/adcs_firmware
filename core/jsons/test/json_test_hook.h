#if defined(__JSON_TEST_HOOK_H__)
#error PLEASE FIX YOUR INCLUDE TREE. TEST_HOOK CAN ONLY BE INCLUDED ONCE PER TRANSLATION UNIT
#endif /* #if defined(__TEST_HOOK_H) */


#ifndef __JSON_TEST_HOOK_H__
#define __JSON_TEST_HOOK_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint8_t OBC_MESSAGE_SIPHON_BUFFER[250];

int OBC_IF_tx(uint8_t *buf, uint_least16_t buflen)
{
    strncpy((char *)OBC_MESSAGE_SIPHON_BUFFER, (char *)buf, buflen);
    return buflen;
}

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __JSON_TEST_HOOK_H__ */

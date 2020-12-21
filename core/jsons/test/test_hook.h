#if defined(__TEST_HOOK_H)
#error PLEASE FIX YOUR INCLUDE TREE. TEST_HOOK CAN ONLY BE INCLUDED ONCE PER TRANSLATION UNIT
#endif /* #if defined(__TEST_HOOK_H) */


#ifndef __TEST_HOOK_H__
#define __TEST_HOOK_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#include <stdint.h>
#include <stdio.h>

int OBC_IF_tx(uint8_t *buf, uint_least16_t buflen)
{
    return printf("%.*s", buflen, buf);
}

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __TEST_HOOK_H__ */

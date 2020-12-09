#ifndef __JSONS_H__
#define __JSONS_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#include <stdint.h>

int json_parse(uint8_t *json, uint_least16_t json_len);

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __JSONS_H__ */

#ifndef __JSONS_H__
#define __JSONS_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#include <stdint.h>

typedef enum
{
    JSON_PARSE_ok,
    JSON_PARSE_format_err,
    JSON_PARSE_unsupported,
} JSON_PARSE_t;

/**
 * @brief Parse a json and execute commands based on the key : value pairs
 *
 * @param json nul-terminated string in json format
 * @param json_strlen
 * @return one of JSON_PARSE_t
 */
JSON_PARSE_t json_parse(uint8_t *json);

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __JSONS_H__ */

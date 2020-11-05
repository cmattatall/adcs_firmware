#ifndef JSON_H_
#define JSON_H_

#ifndef __JSON_H_
#define __JSON_H_
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

#define JSON_STANDALONE_TOKENS

/**
 * JSON type identifier. Basic types are:
 *  - Object
 *  - Array
 *  - String
 *  - Other primitive: number, boolean (true/false) or null
 */
typedef enum
{
    JSON_PRIMITIVE = 0,
    JSON_OBJECT    = 1,
    JSON_ARRAY     = 2,
    JSON_STRING    = 3
} jsontype_t;

typedef enum
{
    /* Not enough tokens were provided */
    JSON_ERROR_NOMEM = -1,

    /* Invalid character inside JSON string */
    JSON_ERROR_INVAL = -2,

    /* The string is not a full JSON packet, more bytes expected */
    JSON_ERROR_PART = -3
} jsonerr_t;


/**
 * JSON token description.
 * @param       type    type (object, array, string etc.)
 * @param       start   start position in JSON data string
 * @param       end     end position in JSON data string
 */
typedef struct
{
#if defined(JSON_STANDALONE_TOKENS)
    char *json;
#endif /* #if defined(JSON_STANDALONE_TOKENS) */
    jsontype_t type;
    int        start;
    int        end;
    int        size;
#if defined(JSON_PARENT_LINKS)
    int parent;
#endif /* #if defined(JSON_PARENT_LINKS) */
} jsontok_t;

/**
 * JSON parser. Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string
 */
typedef struct
{
    char *       json;     /* ptr to start of json string */
    unsigned int pos;      /* offset in the JSON string */
    unsigned int toknext;  /* next token to allocate */
    int          toksuper; /* superior token node, e.g parent object or array */
} json_parser;


/**
 * @brief construct json parser over and array of tokens
 *
 * @return the constructed json parser
 */
json_parser json_init(void);


/**
 * @brief Parse a json
 *
 * @param parser json parser
 * @param js json string
 * @param len length of json string
 * @param tokens array of jsontok (provided by caller)
 * @param num_tokens max number of tokens to parse
 * @return jsonerr_t parse status
 */
jsonerr_t json_parse(json_parser *parser, const char *js, size_t len,
                     jsontok_t *tokens, unsigned int num_tokens);


/**
 * @brief get the token length of a jsontok_t;
 *
 * @param tok
 * @return uint32_t the length of the token
 */
uint_least16_t json_toklen(const jsontok_t *tok);


#if defined(JSON_STANDALONE_TOKENS)

/**
 * @brief Compare a json token with a nul-terminated string
 *
 * @param str char array
 * @param tok the jsontok to compare against
 * @return true if equal
 * @return false if not equal
 */
bool json_tokcmp(const char *str, const jsontok_t *tok);


/**
 * @brief Compare no more than n bytes between a string and a jsontok
 *
 * @param str the string to compare against
 * @param tok the jsontok to compare against
 * @param n max number of bytes to compare
 * @return true if equal within bytecount
 * @return false if not equal within bytecount
 */
bool json_tokncmp(const char *str, const jsontok_t *tok, uint_least16_t n);


/**
 * @brief Copy a jsontok_t into a buffer
 *
 * @param dst the destination byte buffer
 * @param bufsize size of desintation buffer
 * @param tkn json token to copy
 * @return char* NULL on error, otherwise, address of destination
 */
char *json_tokcpy(char *dst, uint_least16_t bufsize, const jsontok_t *tkn);


/**
 * @brief Copy a jsontok_t into a buffer
 *
 * @param dst the destination byte buffer
 * @param bufsize size of desintation buffer
 * @param tkn json token to copy
 * @return char* NULL on error, otherwise, address of destination
 */
char *json_tokncpy(char *dst, uint_least16_t bufsize, const jsontok_t *tkn,
                   uint_least16_t n);

#else /* JSON_STANDALONE_TOKENS not defined */

/**
 * @brief Compare a token with a nul-terminated string
 *
 * @param str char array
 * @param json json string
 * @param tok jsontok_t instance of a json token parsed from the json string
 * @return true if equal
 * @return false if not equal
 */
bool json_tokcmp(const char *str, const uint8_t *json, const jsontok_t *tok);


/**
 * @brief Compare no more than n bytes between a string and a jsontok
 *
 * @param str the string to compare against
 * @param json the json string referenced by jsontok tok
 * @param tok the jsontok to compare against
 * @param n max number of bytes to compare
 * @return true if equal within bytecount
 * @return false if not equal within bytecount
 */
bool json_tokncmp(const char *str, const uint8_t *json, const jsontok_t *tok,
                  uint_least16_t n);

/**
 * @brief Copy a jsontok_t into a buffer
 *
 * @param buf the destination byte buffer
 * @param bufsize size of destination buffer
 * @param json json string
 * @param tkn jsontok to copy
 * @return char* NULL on error, else, the destination
 */
char *json_tokcpy(char *dst, uint_least16_t bufsize, const uint8_t *json,
                  const jsontok_t *tkn);

/**
 * @brief Copy a no more than n bytes from a jsontok_t into a buffer
 *
 * @param buf the destination byte buffer
 * @param bufsize size of destination buffer
 * @param json json string
 * @param tkn jsontok to copy
 * @return char* NULL on error, else, the destination
 */
char *json_tokncpy(char *dst, uint_least16_t bufsize, const uint8_t *json,
                   const jsontok_t *tkn, uint_least16_t n);

#endif /* #if defined(JSON_STANDALONE_TOKENS) */


/**
 * @brief Check if a jsontok array constitutes a valid json structure
 *
 * @param tokens token array
 * @param tcnt number of tokens parsed from some arbitrary json string
 * @return true valid
 * @return false invalid
 */
bool isValidJson(const jsontok_t *tokens, uint_least8_t tcnt);


/**
 * @brief Utility wrapper for printing the type name of a jsontok as a string
 *
 * @param type the type to stringify
 * @return char* the type name as a string
 */
char *json_toktypename(jsontype_t type);

#ifdef __cplusplus
}
#endif
#endif /* __JSON_H_ */

#endif /* JSON_H_ */

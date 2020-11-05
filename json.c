/**
 * @file json.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Statically allocated JSON parser for embedded systems
 * @version 0.3
 * @date 2020-11-05
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 * @todo Portability for various mcu word sizes
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "json.h"

#define JSON_ASCII_CHAR_LOWEST_VALUE 32   /* ' ' space */
#define JSON_ASCII_CHAR_HIGHEST_VALUE 127 /* DEL */


/**
 * @brief Allocate fresh token from the token pool
 *
 * @param parser
 * @param tokens
 * @param num_tokens
 * @return jsontok_t*
 */
static jsontok_t *json_alloc_token(json_parser *parser, jsontok_t *tokens,
                                   size_t num_tokens);


/**
 * @brief Fill json_token type and boundaries
 *
 * @param token the json token to populate
 * @param type the token type
 * @param start stard index
 * @param end end index
 *
 * @return 0 on success, 1 on failure
 */
static int json_fill_token(jsontok_t *token, jsontype_t type, int start,
                           int end);


/**
 * @brief Parse and fill next available token as json primitive
 *
 * @param parser the json parser
 * @param js the json string
 * @param len length of json string
 * @param tokens token array (caller provided)
 * @param num_tokens maximum number of tokens to parse
 * @return jsonerr_t parse status
 */
static jsonerr_t json_parse_primitive(json_parser *parser, const char *js,
                                      size_t len, jsontok_t *tokens,
                                      size_t num_tokens);


/**
 * @brief Parse and fill next available json token as a json string
 *
 * @param parser the json parser
 * @param js json string
 * @param len length of json string
 * @param tokens token array (caller provided)
 * @param num_tokens max number of tokens to parse
 * @return jsonerr_t parse status
 */
static jsonerr_t json_parse_string(json_parser *parser, const char *js,
                                   size_t len, jsontok_t *tokens,
                                   size_t num_tokens);


char *json_toktypename(jsontype_t type)
{
    static const char *jsontok_type_names[] = {
        [JSON_PRIMITIVE] = "JSON_PRIMITIVE",
        [JSON_OBJECT]    = "JSON_OBJECT",
        [JSON_ARRAY]     = "JSON_ARRAY",
        [JSON_STRING]    = "JSON_STRING",
    };
    switch (type)
    {
        case JSON_PRIMITIVE:
        case JSON_OBJECT:
        case JSON_ARRAY:
        case JSON_STRING:
            return jsontok_type_names[type];
        default:
            return NULL;
    }
}


char *json_jsonerr_messages(jsonerr_t err)
{
    static const char *jsonerr_messages[] = {
        [0] = "Not enough jsontok_t tokens were provided",
        [1] = "Invalid character inside JSON string",
        [2] = "The string is not a full JSON packet, more bytes expected",
    };
    switch (err)
    {
        case JSON_ERROR_NOMEM:
        {
            return jsonerr_messages[0];
        }
        break;
        case JSON_ERROR_INVAL:
        {
            return jsonerr_messages[1];
        }
        break;
        case JSON_ERROR_PART:
        {
            return jsonerr_messages[2];
        }
        break;
        default:
        {
            return NULL;
        }
        break;
    }
}

uint_least16_t json_toklen(const jsontok_t *tok)
{
    uint_least16_t len = 0;
    if (tok != NULL)
    {
        uint_least64_t tokstart = tok->start;
        uint_least64_t tokend   = tok->end;
        if (tokend >= 0 && tokstart >= 0)
        {
            if (tokend - tokstart < UINT16_MAX)
            {
                len = tokend - tokstart;
            }
        }
    }
    return len;
}


#if defined(JSON_STANDALONE_TOKENS)

bool json_tokcmp(const char *str, const jsontok_t *tok)
{
    bool result = false;
    if (str == NULL)
    {
        if (tok != NULL && tok->json == NULL)
        {
            result = true;
        }
    }
    else if (tok != NULL && tok->json == NULL)
    {
        if (str == NULL)
        {
            result = true;
        }
    }
    else
    {
        uint_least16_t least_size = json_toklen(tok);
        uint_least16_t slen       = strlen(str);
        if (least_size < slen)
        {
            least_size = slen;
        }

        /* actually compare them */
        if (strncmp((const char *)str, (char *)&tok->json[tok->start],
                    least_size) == 0)
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }
    return result;
}

bool json_tokncmp(const char *str, const jsontok_t *tok, uint_least16_t n)
{
    bool result = false;
    if (str != NULL && tok != NULL && tok->json != NULL)
    {
        uint_least16_t least_size = json_toklen(tok);
        uint_least16_t slen       = strlen(str);
        if (least_size < slen)
        {
            least_size = slen;
        }

        if (least_size < n)
        {
            least_size = n;
        }

        /* actually compare them */
        if (strncmp((const char *)str, (char *)&tok->json[tok->start],
                    least_size) == 0)
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }
}


char *json_tokcpy(char *dst, uint_least16_t bufsize, const jsontok_t *tkn)
{
    char *result = NULL;
    if (dst != NULL && tkn != NULL && tkn->json != NULL)
    {
        uint_least16_t copy_count = json_toklen(tkn);
        if (copy_count > bufsize)
        {
            copy_count = bufsize;
        }
        result = strncpy(dst, (char *)&tkn->json[tkn->start], copy_count);
    }
    return result;
}

char *json_tokncpy(char *dst, uint_least16_t bufsize, const jsontok_t *tkn,
                   uint_least16_t n)
{
    char *         result = NULL;
    uint_least16_t count  = bufsize;
    if (bufsize > n)
    {
        count = n;
    }
    result = json_tokcpy(dst, count, tkn);
    return result;
}

#else /* JSON_STANDALONE_TOKENS is not defined */


bool json_tokncmp(const char *str, const uint8_t *json, const jsontok_t *tok,
                  uint_least16_t n)
{
    bool result = false;
    if (str != NULL && json != NULL && tok != NULL)
    {
        uint_least16_t least_size = json_toklen(tok);
        uint_least16_t slen       = strlen(str);
        if (least_size < slen)
        {
            least_size = slen;
        }

        if (least_size < n)
        {
            least_size = n;
        }

        /* actually compare them */
        if (strncmp((const char *)str, (char *)&json[tok->start], least_size) ==
            0)
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }
}


bool json_tokcmp(const char *str, const uint8_t *json, const jsontok_t *tok)
{
    bool result = false;
    if (str == NULL)
    {
        if (json == NULL)
        {
            result = true;
        }
        /* Fall to end, default return is false */
    }
    else if (json == NULL)
    {
        if (str == NULL)
        {
            result = true;
        }
        /* Fall to end, default return is false */
    }
    else
    {
        uint_least16_t least_size = json_toklen(tok);
        uint_least16_t slen       = strlen(str);
        if (least_size < slen)
        {
            least_size = slen;
        }

        /* actually compare them */
        if (strncmp((const char *)str, (char *)&json[tok->start], least_size) ==
            0)
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }
    return result;
}

char *json_tokcpy(char *dst, uint_least16_t bufsize, const uint8_t *json,
                  const jsontok_t *tkn)
{
    char *result = NULL;
    if (dst != NULL && json != NULL && tkn != NULL)
    {
        uint_least16_t copy_count = json_toklen(tkn);
        if (copy_count > bufsize)
        {
            copy_count = bufsize;
        }
        result = strncpy(dst, (char *)&json[tkn->start], copy_count);
    }
    return result;
}


char *json_tokncpy(char *dst, uint_least16_t bufsize, const uint8_t *json,
                   const jsontok_t *tkn, uint_least16_t n)
{
    char *         result = NULL;
    uint_least16_t count  = bufsize;
    if (bufsize > n)
    {
        count = n;
    }
    result = json_tokcpy(dst, count, json, tkn);
    return result;
}

#endif /* #if defined(JSON_STANDALONE_TOKENS) */


bool isValidJson(const jsontok_t *tokens, uint_least8_t tcnt)
{
    bool isValid = false;
    if (tokens != NULL)
    {
        if (tcnt > 1)
        {
            if (tokens[0].type == JSON_OBJECT)
            {
                isValid = true;
            }

            if (tcnt == 2) /* Cannot have something like { "myKey" } */
            {
                if (tokens[1].type == JSON_ARRAY)
                {
                    /* however, { [ ] } is still technically valid */
                    isValid = true;
                }
            }
            else
            {
                /* First key in a json must be a "string" */
                if (tcnt > 2)
                {
                    if (tokens[1].type == JSON_STRING)
                    {
                        isValid = true;
                    }
                }
            }
        }
    }
    return isValid;
}


static jsontok_t *json_alloc_token(json_parser *parser, jsontok_t *tokens,
                                   size_t num_tokens)
{
    jsontok_t *tok;
    if (parser->toknext >= num_tokens)
    {
        return NULL;
    }
    tok        = &tokens[parser->toknext++];
    tok->start = tok->end = -1;
    tok->size             = 0;
#if defined(JSON_PARENT_LINKS)
    tok->parent = -1;
#endif /* #if defined(JSON_PARENT_LINKS) */

#if defined(JSON_STANDALONE_TOKENS)
    tok->json = parser->json;
#endif /* #if defined(JSON_STANDALONE_TOKENS) */
    return tok;
}


static int json_fill_token(jsontok_t *token, jsontype_t type, int start,
                           int end)
{
    if (token != NULL)
    {
        token->type  = type;
        token->start = start;
        token->end   = end;
        token->size  = 0;
        return 0;
    }
    else
    {
        return 1;
    }
}


static jsonerr_t json_parse_primitive(json_parser *parser, const char *js,
                                      size_t len, jsontok_t *tokens,
                                      size_t num_tokens)
{
    jsontok_t *token;
    int        start;
    start = parser->pos;

    for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++)
    {
        switch (js[parser->pos])
        {
#ifndef JSON_STRICT
            /* In strict mode primitive must be followed by "," or
             * "}" or "]" */
            case ':':
#endif
            case '\t':
            case '\r':
            case '\n':
            case ' ':
            case ',':
            case ']':
            case '}':
            {
                goto found;
            }
            break;
        }
        if (js[parser->pos] < JSON_ASCII_CHAR_LOWEST_VALUE ||
            js[parser->pos] >= JSON_ASCII_CHAR_HIGHEST_VALUE)
        {
            parser->pos = start;
            return JSON_ERROR_INVAL;
        }
    }
#ifdef JSON_STRICT
    /* In strict mode primitive must be followed by a
     * comma/object/array */
    parser->pos = start;
    return JSON_ERROR_PART;
#endif

found:
    if (tokens == NULL)
    {
        parser->pos--;
        return (jsonerr_t)0;
    }
    token = json_alloc_token(parser, tokens, num_tokens);
    if (token == NULL)
    {
        parser->pos = start;
        return JSON_ERROR_NOMEM;
    }
    json_fill_token(token, JSON_PRIMITIVE, start, parser->pos);
#ifdef JSON_PARENT_LINKS
    token->parent = parser->toksuper;
#endif
    parser->pos--;
    return (jsonerr_t)0;
}


static jsonerr_t json_parse_string(json_parser *parser, const char *js,
                                   size_t len, jsontok_t *tokens,
                                   size_t num_tokens)
{
    jsontok_t *token;

    int start = parser->pos;

    parser->pos++;

    /* Skip starting quote */
    for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++)
    {
        char c = js[parser->pos];

        /* Quote: end of string */
        if (c == '\"')
        {
            if (tokens == NULL)
            {
                return (jsonerr_t)0;
            }
            token = json_alloc_token(parser, tokens, num_tokens);
            if (token == NULL)
            {
                parser->pos = start;
                return JSON_ERROR_NOMEM;
            }
            json_fill_token(token, JSON_STRING, start + 1, parser->pos);
#ifdef JSON_PARENT_LINKS
            token->parent = parser->toksuper;
#endif
            return (jsonerr_t)0;
        }

        /* Backslash: Quoted symbol expected */
        if (c == '\\' && parser->pos + 1 < len)
        {
            int i;
            parser->pos++;
            switch (js[parser->pos])
            {
                /* Allowed escaped symbols */
                case '\"':
                case '/':
                case '\\':
                case 'b':
                case 'f':
                case 'r':
                case 'n':
                case 't':
                {
                }
                break;
                /* Allows escaped symbol \uXXXX */
                case 'u':
                {
                    parser->pos++;
                    for (i = 0;
                         i < 4 && parser->pos < len && js[parser->pos] != '\0';
                         i++)
                    {
                        /* If it isn't a hex character we have an
                         * error */
                        if (!isxdigit(js[parser->pos]))
                        {
                            parser->pos = start;
                            return JSON_ERROR_INVAL;
                        }
                        parser->pos++;
                    }
                    parser->pos--;
                }
                break;
                /* Unexpected symbol */
                default:
                {
                    parser->pos = start;
                    return JSON_ERROR_INVAL;
                }
                break;
            }
        }
    }
    parser->pos = start;
    return JSON_ERROR_PART;
}


jsonerr_t json_parse(json_parser *parser, const char *js, size_t len,
                     jsontok_t *tokens, unsigned int num_tokens)
{
    jsonerr_t  r;
    int        i;
    jsontok_t *token;
    int        count = 0;

    for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++)
    {
        char       c;
        jsontype_t type;

        c = js[parser->pos];
        switch (c)
        {
            case '{':
            case '[':
                count++;
                if (tokens == NULL)
                {
                    break;
                }
                token = json_alloc_token(parser, tokens, num_tokens);
                if (token == NULL)
                    return JSON_ERROR_NOMEM;
                if (parser->toksuper != -1)
                {
                    tokens[parser->toksuper].size++;
#ifdef JSON_PARENT_LINKS
                    token->parent = parser->toksuper;
#endif
                }
                token->type      = (c == '{' ? JSON_OBJECT : JSON_ARRAY);
                token->start     = parser->pos;
                parser->toksuper = parser->toknext - 1;
                break;
            case '}':
            case ']':
                if (tokens == NULL)
                    break;
                type = (c == '}' ? JSON_OBJECT : JSON_ARRAY);
#ifdef JSON_PARENT_LINKS
                if (parser->toknext < 1)
                {
                    return JSON_ERROR_INVAL;
                }
                token = &tokens[parser->toknext - 1];
                for (;;)
                {
                    if (token->start != -1 && token->end == -1)
                    {
                        if (token->type != type)
                        {
                            return JSON_ERROR_INVAL;
                        }
                        token->end       = parser->pos + 1;
                        parser->toksuper = token->parent;
                        break;
                    }
                    if (token->parent == -1)
                    {
                        break;
                    }
                    token = &tokens[token->parent];
                }
#else
                for (i = parser->toknext - 1; i >= 0; i--)
                {
                    token = &tokens[i];
                    if (token->start != -1 && token->end == -1)
                    {
                        if (token->type != type)
                        {
                            return JSON_ERROR_INVAL;
                        }
                        parser->toksuper = -1;
                        token->end       = parser->pos + 1;
                        break;
                    }
                }
                /* Error if unmatched closing bracket */
                if (i == -1)
                    return JSON_ERROR_INVAL;
                for (; i >= 0; i--)
                {
                    token = &tokens[i];
                    if (token->start != -1 && token->end == -1)
                    {
                        parser->toksuper = i;
                        break;
                    }
                }
#endif
                break;
            case '\"':
            {
                r = json_parse_string(parser, js, len, tokens, num_tokens);
                if (r < 0)
                {
                    return r;
                }
                count++;
                if (parser->toksuper != -1 && tokens != NULL)
                {
                    tokens[parser->toksuper].size++;
                }
            }
            break;
            case '\t':
            case '\r':
            case '\n':
            case ' ':
            {
            }
            break;
            case ':':
            {
                parser->toksuper = parser->toknext - 1;
            }
            break;
            case ',':
            {
                if (tokens != NULL &&
                    tokens[parser->toksuper].type != JSON_ARRAY &&
                    tokens[parser->toksuper].type != JSON_OBJECT)
                {
#ifdef JSON_PARENT_LINKS
                    parser->toksuper = tokens[parser->toksuper].parent;
#else
                    for (i = parser->toknext - 1; i >= 0; i--)
                    {
                        if (tokens[i].type == JSON_ARRAY ||
                            tokens[i].type == JSON_OBJECT)
                        {
                            if (tokens[i].start != -1 && tokens[i].end == -1)
                            {
                                parser->toksuper = i;
                                break;
                            }
                        }
                    }
#endif
                }
            }
            break;
#ifdef JSON_STRICT
            /* In strict mode primitives are: numbers and booleans
             */
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case 't':
            case 'f':
            case 'n':
                /* And they must not be keys of the object */
                if (tokens != NULL)
                {
                    jsontok_t *t = &tokens[parser->toksuper];
                    if (t->type == JSON_OBJECT ||
                        (t->type == JSON_STRING && t->size != 0))
                    {
                        return JSON_ERROR_INVAL;
                    }
                }
#else
            /* In non-strict mode every unquoted value is a
             * primitive */
            default:
#endif
                r = json_parse_primitive(parser, js, len, tokens, num_tokens);
                if (r < 0)
                    return r;
                count++;
                if (parser->toksuper != -1 && tokens != NULL)
                    tokens[parser->toksuper].size++;
                break;

#ifdef JSON_STRICT
            /* Unexpected char in strict mode */
            default:
                return JSON_ERROR_INVAL;
#endif
        }
    }

    for (i = parser->toknext - 1; i >= 0; i--)
    {
        /* Unmatched opened object or array */
        if (tokens[i].start != -1 && tokens[i].end == -1)
        {
            return JSON_ERROR_PART;
        }
    }

    return (jsonerr_t)count;
}


json_parser json_init(void)
{
    json_parser parser;
    parser.pos      = 0;
    parser.toknext  = 0;
    parser.toksuper = -1;
    parser.json     = NULL;
    return parser;
}

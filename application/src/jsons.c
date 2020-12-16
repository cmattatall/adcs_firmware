/**
 * @file jsons.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief JSON ADCS payload parsing file for ADCS firmware
 * @version 0.1
 * @date 2020-12-09
 *
 * @copyright Copyright (c) 2020 DSS - LORIS project
 *
 * @note
 */
#include "targets.h"

#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "jtok.h"
#include "obc_interface.h"
#include "jsons.h"
#include "version.h"

#include "reaction_wheels.h"

#define BASE_10 10
#define JSON_TKN_CNT 250

static jtoktok_t     tkns[JSON_TKN_CNT];
static jtok_parser_t parser;
static char          value_holder[50];

typedef uint_fast16_t token_index_t;

typedef struct
{
    char key[50];
    void *(*handler)(void *);
} json_parse_table_item;


/* PUT FUNCTION PROTOTYPES HERE */
static void *parse_hardware_json(void *args);
static void *parse_firmware_json(void *args);
static void *parse_pwm_rw_x(void *args);

static void *parse_test_json(void *args)
{
    token_index_t *t = (token_index_t *)args;
    *t += 1;
    if (jtok_tokcmp("1", &tkns[*t]))
    {
        OBC_IF_printf("{\"test\":\"ok\"}");
        return NULL;
    }
    else
    {
        return (void *)t;
    }
}


/* clang-format off */
static const json_parse_table_item json_parse_table[] = {
    {.key = "test",      .handler = parse_test_json},
    {.key = "fwVersion", .handler = parse_firmware_json},
    {.key = "hwVersion", .handler = parse_hardware_json},
    {.key = "pwm_rw_x",  .handler = parse_pwm_rw_x},
};
/* clang-format on */

int json_parse(uint8_t *json, uint_least16_t json_len)
{
    CONFIG_ASSERT(json != NULL);

    int json_parse_status = 0;

    parser = jtok_new_parser((char *)json);

    int jtok_retval = jtok_parse(&parser, tkns, JSON_TKN_CNT);
    if (jtok_retval != JTOK_PARSE_STATUS_PARSE_OK)
    {
        json_parse_status = jtok_retval;
        memset(&parser, 0, sizeof(parser));
        memset(tkns, 0, sizeof(tkns));
    }
    else
    {

        token_index_t t; /* token index */
        token_index_t k; /* key index for json table */
        t = 0;
        if (isValidJson(tkns, JSON_TKN_CNT))
        {
            /* Go through command table and check if we have a registered
             * command for the key */
            t++;
            for (k = 0;
                 k < sizeof(json_parse_table) / sizeof(*json_parse_table); k++)
            {
                /*
                 * If we have a command for the current key,
                 * execute the command handler
                 */
                if (jtok_tokcmp(json_parse_table[k].key, &tkns[t]))
                {
                    if (NULL != json_parse_table[k].handler)
                    {
                        json_parse_table[k].handler(&t);
                    }
                }
            }
        }
        else
        {
            json_parse_status = 1;
        }
    }
    return json_parse_status;
}


static void *parse_firmware_json(void *args)
{
    token_index_t *t = (token_index_t *)args;
    *t += 1;
    if (jtok_tokcmp("read", &tkns[*t]))
    {
        OBC_IF_printf("{\"fwVersion\" : \"%s\"}", FW_VERSION);
        return NULL;
    }
    else
    {
        return (void *)t;
    }
}


static void *parse_hardware_json(void *args)
{
    token_index_t *t = (token_index_t *)args;
    *t += 1;
    if (jtok_tokcmp("read", &tkns[*t]))
    {
        OBC_IF_printf("{\"hwVersion\" : \"%s\"}", HW_VERSION);
        return NULL;
    }
    else
    {
        return (void *)t;
    }
}


static void *parse_pwm_rw_x(void *args)
{
    token_index_t *t = (token_index_t *)args;
    CONFIG_ASSERT(*t < JSON_TKN_CNT);
    *t += 1; /* don't do ++ because * has higher precedence than ++ */
    if (jtok_tokcmp("read", &tkns[*t]))
    {
        pwm_t current_x_pwm = get_reaction_wheel_pwm(REACTION_WHEEL_x);
        OBC_IF_printf("{\"pwm_rw_x\" : \"%d\"}", current_x_pwm);
    }
    else if (jtok_tokcmp("write", &tkns[*t]))
    {
        *t += 1;
        if (jtok_tokcmp("value", &tkns[*t]))
        {
            /* getting values from JSON is a little unelegant in C ... */
            *t += 1;
            memset(value_holder, '\0', sizeof(value_holder));
            jtok_tokcpy(value_holder, sizeof(value_holder), &tkns[*t]);
            char *endptr    = value_holder;
            pwm_t new_value = (pwm_t)strtoul(value_holder, &endptr, BASE_10);
            if (endptr != &tkns[*t].json[tkns[*t].end])
            {
                /* error parsing the value - couldn't reach end of token */
                return NULL;
            }
            else
            {
                set_reaction_wheel_pwm(REACTION_WHEEL_x, new_value);
            }
            memset(value_holder, 0, sizeof(value_holder));
        }
        else
        {
            /* we were missing data from the payload.
             * eg : { "pwm_rw_x" : "write" } <-- notice "value" : 55 is missing
             */
            return NULL;
        }
    }
    else
    {
        /* error handle ?? */
    }
    return t;
}
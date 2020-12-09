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
 * @todo
 */
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define JTOK_STANDALONE_TOKENS
#include "jtok.h"
#include "jsons.h"
#include "utils.h"
#include "obc_interface.h"
#include "version.h"

#include "reaction_wheels.h"

#define BASE_10 10
#define JSON_TKN_CNT 250


static jtoktok_t     tkns[JSON_TKN_CNT];
static jtok_parser_t parser;
static uint8_t       token_value_holder[50];

typedef uint_fast16_t token_index_t;
typedef void *(*generic_function)(void *);
typedef struct
{
    char key[50];
    void *(*handler)(void *);
} json_parse_table_item;

/* PUT FUNCTION PROTOTYPES HERE */
static generic_function send_hardware_json(void *args);
static generic_function send_firmware_json(void *args);
static generic_function handle_pwm_rw_x(void *args);

static const json_parse_table_item json_parse_table[] = {
    {.key = "fwVersion", .handler = send_firmware_json},
    {.key = "hwVersion", .handler = send_hardware_json},
    {.key = "pwm_rw_x", .handler = handle_pwm_rw_x}};

static generic_function send_hardware_json(void *args)
{
    OBC_printf("{\"fwVersion\" : \"%s\"}%c", FW_VERSION, OBC_MSG_DELIM);
}

static generic_function send_firmware_json(void *args)
{
    OBC_printf("{\"hwVersion\" : \"%s\"}%c", HW_VERSION, OBC_MSG_DELIM);
}


static generic_function handle_pwm_rw_x(void *args)
{
    token_index_t *t = (token_index_t *)args;
    CONFIG_ASSERT(t < JSON_TKN_CNT);
    *t++;

    if (jtok_tokcmp("read", &tkns[*t]))
    {
#warning THIS IS AN EXAMPLE CODE SEGMENT

        /* using a var called tmp so code fits in 1 line */
        pwm_value_t current_x_pwm = get_reaction_wheel_pwm(REACTION_WHEEL_x);
        OBC_printf("{\"pwm_rw_x\" : \"%s\"}%c", current_x_pwm, OBC_MSG_DELIM);
    }
    else if (jtok_tokcmp("write", &tkns[*t]))
    {
        *t++;
        if (jtok_tokcmp("value", &tkns[*t]))
        {
            *t++;

            /* getting values from JSON is a little unelegant in C ... */
            jtok_tokcpy(token_value_holder, sizeof(token_value_holder),
                        &tkns[*t]);

            char *      endptr = token_value_holder;
            pwm_value_t new_value =
                (pwm_value_t)strtoul(token_value_holder, &endptr, BASE_10);

            if (endptr != &tkns[*t].json[tkns[*t].end])
            {
                /* error parsing the value - couldn't reach end of token */
                return NULL;
            }
            else
            {
                set_reaction_wheel_pwm(REACTION_WHEEL_x, new_value);
            }
            memset(token_value_holder, 0, sizeof(token_value_holder));
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

int json_parse(uint8_t *json, uint_least16_t json_len)
{
    CONFIG_ASSERT(json != NULL);

    int json_parse_status = 0;

    int jtok_retval = jtok_parse(&parser, tkns, JSON_TKN_CNT);
    if (jtok_retval != JTOK_PARSE_STATUS_PARSE_OK)
    {
        json_parse_status = jtok_retval;
        memset(&parser, 0, sizeof(parser));
        memset(tkns, 0, sizeof(tkns));
    }
    else
    {
        /** @todo USE A TABLE DRIVEN APPROACH. FOR NOW, WE JUST DO GIANT IF-ELSE
         * CHAIN AS PROOF OF CONCEPT */

        token_index_t t; /* token index */
        token_index_t k; /* key index for json table */
        t = 0;
        if (isValidJson(tkns, JSON_TKN_CNT))
        {
            t++;
            for (k = 0;
                 k < sizeof(json_parse_table) / sizeof(*json_parse_table); k++)
            {
                if (jtok_tokcmp(json_parse_table[k].key, &tkns[t]))
                {
                    if (NULL == json_parse_table[k].handler(&t))
                    {
                        /* set status to token index that caused the error */
                        json_parse_status = t;
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

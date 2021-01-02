/**
 * @file jsons.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief JSON ADCS payload parsing file for ADCS firmware
 * @version 0.1
 * @date 2020-12-09
 *
 * @copyright Copyright (c) 2020 DSS - LORIS project
 *
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
#define JSON_TKN_CNT 20
#define JSON_HANDLER_RETVAL_ERROR NULL

typedef uint_fast16_t token_index_t;

typedef void *         json_handler_retval;
typedef token_index_t *json_handler_args;
typedef json_handler_retval (*json_handler)(json_handler_args);

typedef struct
{
    char         key[25];
    json_handler handler;
} json_parse_table_item;


static jtok_tkn_t tkns[JSON_TKN_CNT];
static char       value_holder[50];


/* JSON HANDLER DECLARATIONS */
static void *parse_hardware_json(json_handler_args args);
static void *parse_firmware_json(json_handler_args args);
static void *parse_pwm_rw_x(json_handler_args args);
static void *parse_pwm_rw_y(json_handler_args args);
static void *parse_pwm_rw_z(json_handler_args args);
static void *parse_dir_rw_x(json_handler_args args);
static void *parse_dir_rw_y(json_handler_args args);
static void *parse_dir_rw_z(json_handler_args args);

/* JSON PARSE TABLE */
/* clang-format off */
static const json_parse_table_item json_parse_table[] = {
    {.key = "fwVersion", .handler = parse_firmware_json},
    {.key = "hwVersion", .handler = parse_hardware_json},
    {.key = "pwm_rw_x",  .handler = parse_pwm_rw_x},
    {.key = "pwm_rw_y",  .handler = parse_pwm_rw_y},
    {.key = "pwm_rw_z",  .handler = parse_pwm_rw_z},
    {.key = "dir_rw_x",  .handler = parse_dir_rw_x},
    {.key = "dir_rw_y",  .handler = parse_dir_rw_y},
    {.key = "dir_rw_z",  .handler = parse_dir_rw_z},


};
/* clang-format on */


int json_parse(uint8_t *json)
{
    CONFIG_ASSERT(json != NULL);

    int json_parse_status = 0;

    int jtok_retval = jtok_parse((char *)json, tkns, JSON_TKN_CNT);

    if (jtok_retval != JTOK_PARSE_STATUS_OK)
    {
        json_parse_status = jtok_retval;
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

            unsigned int k_max =
                sizeof(json_parse_table) / sizeof(*json_parse_table);
            for (k = 0; k < k_max; k++)
            {
                /*
                 * If we have a command for the current key,
                 * execute the command handler
                 */
                if (jtok_tokcmp(json_parse_table[k].key, &tkns[t]))
                {
                    if (NULL != json_parse_table[k].handler)
                    {
                        json_handler_retval retval;
                        retval = json_parse_table[k].handler(&t);
                        if (retval == JSON_HANDLER_RETVAL_ERROR)
                        {
                            json_parse_status = -1;
                        }
                    }
                    break;
                }
            }

            /* No match with supported json keys */
            if (k >= k_max)
            {
                json_parse_status = -1;
            }
        }
        else
        {
            json_parse_status = 1;
        }
    }
    return json_parse_status;
}


static void *parse_firmware_json(json_handler_args args)
{
    token_index_t *t = (token_index_t *)args;
    *t += 1;
    if (jtok_tokcmp("read", &tkns[*t]))
    {
        OBC_IF_printf("{\"fwVersion\" : %s}", FW_VERSION);
        return (void *)t;
    }
    else
    {
        return JSON_HANDLER_RETVAL_ERROR;
    }
}


static void *parse_hardware_json(json_handler_args args)
{
    token_index_t *t = (token_index_t *)args;
    *t += 1;
    if (jtok_tokcmp("read", &tkns[*t]))
    {
        OBC_IF_printf("{\"hwVersion\" : %s}", HW_VERSION);
        return (void *)t;
    }
    else
    {
        return JSON_HANDLER_RETVAL_ERROR;
    }
}


static void *parse_pwm_rw_x(json_handler_args args)
{
    token_index_t *t = (token_index_t *)args;
    CONFIG_ASSERT(*t < JSON_TKN_CNT);
    *t += 1; /* don't do ++ because * has higher precedence than ++ */
    if (jtok_tokcmp("read", &tkns[*t]))
    {
        pwm_t current_x_pwm = reacwheel_get_wheel_pwm(REACTION_WHEEL_x);
        OBC_IF_printf("{\"pwm_rw_x\" : %u}", current_x_pwm);
    }
    else if (jtok_tokcmp("write", &tkns[*t]))
    {
        *t += 1;
        if (jtok_tokcmp("value", &tkns[*t]))
        {
            /* getting values from JSON is a little unelegant in C ... */
            *t += 1;


            memset(value_holder, 0, sizeof(value_holder));
            jtok_tokcpy(value_holder, sizeof(value_holder), &tkns[*t]);
            char *endptr    = value_holder;
            pwm_t new_value = (pwm_t)strtoul(value_holder, &endptr, BASE_10);
            if (*endptr != '\0')
            {
                /* error parsing the value - couldn't reach end of token */
                return JSON_HANDLER_RETVAL_ERROR;
            }
            else
            {
                reacwheel_set_wheel_pwm(REACTION_WHEEL_x, new_value);
                OBC_IF_printf("{\"pwm_rw_x\":\"written\"}");
            }
            memset(value_holder, 0, sizeof(value_holder));
        }
        else
        {
            /* we were missing data from the payload.
             * eg : { "pwm_rw_x" : "write" } <-- notice "value" : 55 is missing
             */
            return JSON_HANDLER_RETVAL_ERROR;
        }
    }
    else
    {

        return JSON_HANDLER_RETVAL_ERROR;
    }
    return t;
}


static void *parse_pwm_rw_y(json_handler_args args)
{
    token_index_t *t = (token_index_t *)args;
    CONFIG_ASSERT(*t < JSON_TKN_CNT);
    *t += 1; /* don't do ++ because * has higher precedence than ++ */
    if (jtok_tokcmp("read", &tkns[*t]))
    {
        pwm_t current_y_pwm = reacwheel_get_wheel_pwm(REACTION_WHEEL_y);
        OBC_IF_printf("{\"pwm_rw_y\" : %u}", current_y_pwm);
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
            if (*endptr != '\0')
            {
                /* error parsing the value - couldn't reach end of token */
                return JSON_HANDLER_RETVAL_ERROR;
            }
            else
            {
                reacwheel_set_wheel_pwm(REACTION_WHEEL_y, new_value);
                OBC_IF_printf("{\"pwm_rw_y\":\"written\"}");
            }
            memset(value_holder, 0, sizeof(value_holder));
        }
        else
        {
            /* we were missing data from the payload.
             * eg : { "pwm_rw_y" : "write" } <-- notice "value" : 55 is missing
             */
            return JSON_HANDLER_RETVAL_ERROR;
        }
    }
    else
    {

        return JSON_HANDLER_RETVAL_ERROR;
    }
    return t;
}


static void *parse_pwm_rw_z(json_handler_args args)
{
    token_index_t *t = (token_index_t *)args;
    CONFIG_ASSERT(*t < JSON_TKN_CNT);
    *t += 1; /* don't do ++ because * has higher precedence than ++ */
    if (jtok_tokcmp("read", &tkns[*t]))
    {
        pwm_t current_z_pwm = reacwheel_get_wheel_pwm(REACTION_WHEEL_z);
        OBC_IF_printf("{\"pwm_rw_z\" : %u}", current_z_pwm);
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
            if (*endptr != '\0')
            {
                /* error parsing the value - couldn't reach end of token */
                return JSON_HANDLER_RETVAL_ERROR;
            }
            else
            {
                reacwheel_set_wheel_pwm(REACTION_WHEEL_z, new_value);
                OBC_IF_printf("{\"pwm_rw_z\":\"written\"}");
            }
            memset(value_holder, 0, sizeof(value_holder));
        }
        else
        {
            /*
             * we were missing data from the payload.
             * eg : { "pwm_rw_z" : "write" } <-- notice "value" : 55 is missing
             */
            return JSON_HANDLER_RETVAL_ERROR;
        }
    }
    else
    {

        return JSON_HANDLER_RETVAL_ERROR;
    }
    return t;
}


static void *parse_dir_rw_x(json_handler_args args)
{
    token_index_t *t = (token_index_t *)args;
    CONFIG_ASSERT(*t < JSON_TKN_CNT);
    *t += 1; /* don't do ++ because * has higher precedence than ++ */
    if (jtok_tokcmp("read", &tkns[*t]))
    {
        RW_DIR_t current_dir = reacwheel_get_wheel_pwm(REACTION_WHEEL_x);
        OBC_IF_printf("{\"dir_rw_x\": \"%s\"}", reacwheel_dir_str(current_dir));
    }
    else if (jtok_tokcmp("write", &tkns[*t]))
    {
        *t += 1;
        if (jtok_tokcmp("value", &tkns[*t]))
        {
            /* getting values from JSON is a little unelegant in C ... */
            *t += 1;
            if (jtok_tokcmp("clock", &tkns[*t]))
            {
                reacwheel_set_wheel_dir(REACTION_WHEEL_x, RW_DIR_clockwise);
            }
            else if (jtok_tokcmp("antiClock", &tkns[*t]))
            {
                reacwheel_set_wheel_dir(REACTION_WHEEL_x, RW_DIR_anticlockwise);
            }
            else
            {
                /*
                 * We won't set value because we knows its an invalid command
                 * and can handle it here (rather than rely on caller)
                 */
                return JSON_HANDLER_RETVAL_ERROR;
            }

            OBC_IF_printf("{\"dir_rw_x\":\"written\"}");
        }
        else
        {
            /*
             * we were missing data from the payload.
             * eg : { "dir_rw_x" : "write" } <-- notice "value" missing
             */
            return JSON_HANDLER_RETVAL_ERROR;
        }
    }
    else
    {

        return JSON_HANDLER_RETVAL_ERROR;
    }
    return t;
}


static void *parse_dir_rw_y(json_handler_args args)
{
    token_index_t *t = (token_index_t *)args;
    CONFIG_ASSERT(*t < JSON_TKN_CNT);
    *t += 1; /* don't do ++ because * has higher precedence than ++ */
    if (jtok_tokcmp("read", &tkns[*t]))
    {
        RW_DIR_t current_dir = reacwheel_get_wheel_pwm(REACTION_WHEEL_y);
        switch (current_dir)
        {
            case RW_DIR_clockwise:
            {
                OBC_IF_printf("{\"dir_rw_y\" : \"%s\"}", "clock");
            }
            break;
            case RW_DIR_anticlockwise:
            {
                OBC_IF_printf("{\"dir_rw_y\" : \"%s\"}", "antiClock");
            }
            break;
            case RW_DIR_invalid:
            {
                OBC_IF_printf("{\"dir_rw_y\" : \"%s\"}", "invalid");
            }
            break;
        }
    }
    else if (jtok_tokcmp("write", &tkns[*t]))
    {
        *t += 1;
        if (jtok_tokcmp("value", &tkns[*t]))
        {
            /* getting values from JSON is a little unelegant in C ... */
            *t += 1;
            if (jtok_tokcmp("clock", &tkns[*t]))
            {
                reacwheel_set_wheel_dir(REACTION_WHEEL_y, RW_DIR_clockwise);
            }
            else if (jtok_tokcmp("antiClock", &tkns[*t]))
            {
                reacwheel_set_wheel_dir(REACTION_WHEEL_y, RW_DIR_anticlockwise);
            }
            else
            {
                /*
                 * We won't set value because we knows its an invalid command
                 * and can handle it here (rather than rely on caller)
                 */
                return JSON_HANDLER_RETVAL_ERROR;
            }

            OBC_IF_printf("{\"dir_rw_y\":\"written\"}");
        }
        else
        {
            /*
             * we were missing data from the payload.
             * eg : { "dir_rw_y" : "write" } <-- notice "value" missing
             */
            return JSON_HANDLER_RETVAL_ERROR;
        }
    }
    else
    {

        return JSON_HANDLER_RETVAL_ERROR;
    }
    return t;
}


static void *parse_dir_rw_z(json_handler_args args)
{
    token_index_t *t = (token_index_t *)args;
    CONFIG_ASSERT(*t < JSON_TKN_CNT);
    *t += 1; /* don't do ++ because * has higher precedence than ++ */
    if (jtok_tokcmp("read", &tkns[*t]))
    {
        RW_DIR_t current_dir = reacwheel_get_wheel_pwm(REACTION_WHEEL_z);
        switch (current_dir)
        {
            case RW_DIR_clockwise:
            {
                OBC_IF_printf("{\"dir_rw_z\" : \"%s\"}", "clock");
            }
            break;
            case RW_DIR_anticlockwise:
            {
                OBC_IF_printf("{\"dir_rw_z\" : \"%s\"}", "antiClock");
            }
            break;
            case RW_DIR_invalid:
            {
                OBC_IF_printf("{\"dir_rw_z\" : \"%s\"}", "invalid");
            }
            break;
        }
    }
    else if (jtok_tokcmp("write", &tkns[*t]))
    {
        *t += 1;
        if (jtok_tokcmp("value", &tkns[*t]))
        {
            /* getting values from JSON is a little unelegant in C ... */
            *t += 1;
            if (jtok_tokcmp("clock", &tkns[*t]))
            {
                reacwheel_set_wheel_dir(REACTION_WHEEL_z, RW_DIR_clockwise);
            }
            else if (jtok_tokcmp("antiClock", &tkns[*t]))
            {
                reacwheel_set_wheel_dir(REACTION_WHEEL_z, RW_DIR_anticlockwise);
            }
            else
            {
                /*
                 * We won't set value because we knows its an invalid command
                 * and can handle it here (rather than rely on caller)
                 */
                return JSON_HANDLER_RETVAL_ERROR;
            }

            OBC_IF_printf("{\"dir_rw_z\":\"written\"}");
        }
        else
        {
            /*
             * we were missing data from the payload.
             * eg : { "dir_rw_z" : "write" } <-- notice "value" missing
             */
            return JSON_HANDLER_RETVAL_ERROR;
        }
    }
    else
    {

        return JSON_HANDLER_RETVAL_ERROR;
    }
    return t;
}

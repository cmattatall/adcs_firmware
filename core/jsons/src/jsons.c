/**
 * @file jsons.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief JSON ADCS payload parsing file for ADCS firmware
 * @version 0.3
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
#include "magnetorquers.h"
#include "sun_sensors.h"
#include "magnetometer.h"

#define BASE_10 10
#define JSON_TKN_CNT 20
#define JSON_HANDLER_RETVAL_ERROR NULL

typedef int token_index_t;

typedef void *         json_handler_retval;
typedef token_index_t *json_handler_args;
typedef json_handler_retval (*json_handler)(json_handler_args);

typedef struct
{
    char         key[25];
    json_handler handler;
} json_parse_table_item;


static jtok_tkn_t tkns[JSON_TKN_CNT];
static char       tmp_chrbuf[100];


/* JSON HANDLER DECLARATIONS */
static json_handler_retval parse_hardware_json(json_handler_args args);
static json_handler_retval parse_firmware_json(json_handler_args args);
static json_handler_retval parse_rw_speed(json_handler_args args);
static json_handler_retval parse_rw_current(json_handler_args args);
static json_handler_retval parse_mqtr_volts(json_handler_args args);
static json_handler_retval parse_sunSen(json_handler_args args);
static json_handler_retval parse_magSen(json_handler_args args);
static json_handler_retval parse_imu(json_handler_args args);
static json_handler_retval parse_current(json_handler_args args);


/* JSON PARSE TABLE */
/* clang-format off */
static const json_parse_table_item json_parse_table[] = {
    {.key = "fwVersion",  .handler = parse_firmware_json},
    {.key = "hwVersion",  .handler = parse_hardware_json},
    {.key = "rw_speed",   .handler = parse_rw_speed},
    {.key = "rw_current", .handler = parse_rw_current},
    {.key = "mqtr_volts", .handler = parse_mqtr_volts},
    {.key = "sunSen",     .handler = parse_sunSen},
    {.key = "magSen",     .handler = parse_magSen},
    {.key = "imu",        .handler = parse_imu},
    {.key = "current",    .handler = parse_current},
};
/* clang-format on */


int json_parse(uint8_t *json)
{
    CONFIG_ASSERT(json != NULL);

    int json_parse_status = 0;

    int jtok_retval;
    jtok_retval = jtok_parse((char *)json, tkns, JSON_TKN_CNT);

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

            int k_max = sizeof(json_parse_table) / sizeof(*json_parse_table);
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


static json_handler_retval parse_firmware_json(json_handler_args args)
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


static json_handler_retval parse_hardware_json(json_handler_args args)
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


static json_handler_retval parse_rw_speed(json_handler_args args)
{
    token_index_t *t = (token_index_t *)args;
    CONFIG_ASSERT(*t < JSON_TKN_CNT);
    *t += 1; /* Advance json token index */


    if (jtok_tokcmp("read", &tkns[*t]))
    {
        memset(tmp_chrbuf, 0, sizeof(tmp_chrbuf));

        RW_config_to_string(tmp_chrbuf, sizeof(tmp_chrbuf));
        OBC_IF_printf("{\"rw_speed\": %s}", tmp_chrbuf);
    }
    else if (jtok_tokcmp("write", &tkns[*t]))
    {
        *t += 1;
        if (jtok_tokcmp("value", &tkns[*t]))
        {
            *t += 1;
            if (tkns[*t].type == JTOK_ARRAY)
            {
                *t += 1; /* Advance token index to the first element of arr */
                const jtok_tkn_t *tkn;
                unsigned int      i           = 0;
                token_index_t *   arr_tkn_idx = t;

                /* Traverse sibling tree of array elements */
                do
                {
                    tkn = &tkns[*t];
                    memset(tmp_chrbuf, 0, sizeof(tmp_chrbuf));
                    jtok_tokcpy(tmp_chrbuf, sizeof(tmp_chrbuf), &tkns[*t]);
                    char *endptr = tmp_chrbuf;
                    int   new_speed;
                    new_speed = (int)strtoul(tmp_chrbuf, &endptr, BASE_10);
                    if (*endptr == '\0')
                    {
                        i++;
                        switch (i)
                        {
                            case 1: {
                                RW_set_speed_rph(REAC_WHEEL_x, new_speed);
                            }
                            break;
                            case 2: {
                                RW_set_speed_rph(REAC_WHEEL_y, new_speed);
                            }
                            break;
                            case 3: {
                                RW_set_speed_rph(REAC_WHEEL_z, new_speed);
                            }
                            break;
                            default: {
                                return JSON_HANDLER_RETVAL_ERROR;
                            }
                            break;
                        }
                    }
                    else
                    {
                        /*
                         * error parsing the value
                         * - couldn't reach end of token
                         */
                        return JSON_HANDLER_RETVAL_ERROR;
                    }
                    memset(tmp_chrbuf, 0, sizeof(tmp_chrbuf));

                    *t = tkn->sibling;
                } while (*t != NO_SIBLING_IDX);


                if (i != NUM_REACTION_WHEELS)
                {
                    /* Array didn't contain speed values for all the params
                     * eg : [ 12, 34] <-- missing third value for rw_z */
                    OBC_IF_printf("{\"rw_speed\": \"write error\"}");
                    return JSON_HANDLER_RETVAL_ERROR;
                }
                else
                {
                    OBC_IF_printf("{\"rw_speed\": \"set\"}");
                    t = arr_tkn_idx;
                    /*
                     * After we walk past last element of
                     * array, *t will be set to NO_SIBLING_IDX
                     * but we don't want to return an error
                     * value from correct parse of json values
                     */
                }
            }
            else
            {
                return JSON_HANDLER_RETVAL_ERROR;
            }
        }
        else
        {
            /*
             * we were missing data from the payload.
             * eg : { "rw_speed" : "write" } <-- notice "value" : [ NUMBERS ]
             * is missing
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


static json_handler_retval parse_rw_current(json_handler_args args)
{
    token_index_t *t = (token_index_t *)args;
    CONFIG_ASSERT(*t < JSON_TKN_CNT);
    *t += 1; /* Advance json token index */

    if (jtok_tokcmp("read", &tkns[*t]))
    {
        int current_ma_x = RW_measure_current_ma(REAC_WHEEL_x);
        int current_ma_y = RW_measure_current_ma(REAC_WHEEL_y);
        int current_ma_z = RW_measure_current_ma(REAC_WHEEL_z);
        OBC_IF_printf("{\"rw_current\": [ %d, %d, %d]}", current_ma_x,
                      current_ma_y, current_ma_z);
    }
    else
    {
        return JSON_HANDLER_RETVAL_ERROR;
    }
    return t;
}


static json_handler_retval parse_mqtr_volts(json_handler_args args)
{
    token_index_t *t = (token_index_t *)args;
    CONFIG_ASSERT(*t < JSON_TKN_CNT);
    *t += 1; /* Advance json token index */

    if (jtok_tokcmp("read", &tkns[*t]))
    {
        memset(tmp_chrbuf, 0, sizeof(tmp_chrbuf));
        MQTR_config_to_str(tmp_chrbuf, sizeof(tmp_chrbuf));
        OBC_IF_printf("{\"mqtr_volts\": %s}", tmp_chrbuf);
    }
    else if (jtok_tokcmp("write", &tkns[*t]))
    {
        *t += 1;
        if (jtok_tokcmp("value", &tkns[*t]))
        {
            *t += 1;
            if (tkns[*t].type == JTOK_ARRAY)
            {
                *t += 1; /* Advance token index to the first element of arr */
                const jtok_tkn_t *tkn;
                unsigned int      i           = 0;
                token_index_t *   arr_tkn_idx = t;

                /* Traverse sibling tree of array elements */
                do
                {
                    tkn = &tkns[*t];
                    memset(tmp_chrbuf, 0, sizeof(tmp_chrbuf));
                    jtok_tokcpy(tmp_chrbuf, sizeof(tmp_chrbuf), &tkns[*t]);
                    char *endptr = tmp_chrbuf;
                    int   new_voltage;
                    new_voltage = (int)strtoul(tmp_chrbuf, &endptr, BASE_10);
                    if (*endptr == '\0')
                    {
                        i++;
                        switch (i)
                        {
                            /* whoever maintains this in the future,
                             * I'm really sorry about magic numbers,
                             * I had to build this entire codebase by myself
                             * in ~3 months and theres almost a million LOC
                             */
                            case 1: {
                                MQTR_set_coil_voltage_mv(MQTR_x, new_voltage);
                            }
                            break;
                            case 2: {
                                MQTR_set_coil_voltage_mv(MQTR_y, new_voltage);
                            }
                            break;
                            case 3: {
                                MQTR_set_coil_voltage_mv(MQTR_z, new_voltage);
                            }
                            break;
                            default: {
                                return JSON_HANDLER_RETVAL_ERROR;
                            }
                            break;
                        }
                    }
                    else
                    {
                        /*
                         * error parsing the value
                         * - couldn't reach end of token
                         */
                        return JSON_HANDLER_RETVAL_ERROR;
                    }
                    memset(tmp_chrbuf, 0, sizeof(tmp_chrbuf));

                    *t = tkn->sibling;
                } while (*t != NO_SIBLING_IDX);


                if (i != NUM_REACTION_WHEELS)
                {
                    /* Array didn't contain speed values for all the params
                     * eg : [ 12, 34] <-- missing third value for rw_z */
                    OBC_IF_printf("{\"mqtr_volts\": \"write error\"}");
                    return JSON_HANDLER_RETVAL_ERROR;
                }
                else
                {
                    OBC_IF_printf("{\"mqtr_volts\": \"set\"}");
                    t = arr_tkn_idx;
                    /*
                     * After we walk past last element of
                     * array, *t will be set to NO_SIBLING_IDX
                     * but we don't want to return an error
                     * value from correct parse of json values
                     */
                }
            }
            else
            {
                return JSON_HANDLER_RETVAL_ERROR;
            }
        }
        else
        {
            /*
             * we were missing data from the payload.
             * eg : { "rw_speed" : "write" } <-- notice "value" : [ NUMBERS ]
             * is missing
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


static json_handler_retval parse_sunSen(json_handler_args args)
{
    token_index_t *t = (token_index_t *)args;
    CONFIG_ASSERT(*t < JSON_TKN_CNT);
    *t += 1; /* Advance to value of "sunSen" key */
    if (jtok_tokcmp("read", &tkns[*t]))
    {
        *t += 1; /* Advance to next key in parent object */
        if (jtok_tokcmp("face", &tkns[*t]))
        {
            *t += 1; /* Advance to value for "face" key */

            /** @note admittedly this could be refactored to use a table
             * of some sort */
            memset(tmp_chrbuf, 0, sizeof(tmp_chrbuf));
            if (jtok_tokcmp("x+", &tkns[*t]))
            {
                SUNSEN_face_lux_to_string(tmp_chrbuf, sizeof(tmp_chrbuf),
                                          SUNSEN_FACE_x_pos);
                OBC_IF_printf("{ \"sunSen\" : \"x+\", \"lux\" : %s}",
                              tmp_chrbuf);
            }
            else if (jtok_tokcmp("x-", &tkns[*t]))
            {
                SUNSEN_face_lux_to_string(tmp_chrbuf, sizeof(tmp_chrbuf),
                                          SUNSEN_FACE_x_neg);
                OBC_IF_printf("{ \"sunSen\" : \"x-\", \"lux\" : %s}",
                              tmp_chrbuf);
            }
            else if (jtok_tokcmp("y+", &tkns[*t]))
            {
                SUNSEN_face_lux_to_string(tmp_chrbuf, sizeof(tmp_chrbuf),
                                          SUNSEN_FACE_y_pos);
                OBC_IF_printf("{ \"sunSen\" : \"y+\", \"lux\" : %s}",
                              tmp_chrbuf);
            }
            else if (jtok_tokcmp("y-", &tkns[*t]))
            {
                SUNSEN_face_lux_to_string(tmp_chrbuf, sizeof(tmp_chrbuf),
                                          SUNSEN_FACE_y_neg);
                OBC_IF_printf("{ \"sunSen\" : \"y-\", \"lux\" : %s}",
                              tmp_chrbuf);
            }
            else if (jtok_tokcmp("z+", &tkns[*t]))
            {
                SUNSEN_face_lux_to_string(tmp_chrbuf, sizeof(tmp_chrbuf),
                                          SUNSEN_FACE_z_pos);
                OBC_IF_printf(
                    "{ \"sunSen\" : \"z+\", \"lux\" : %s , \"temp\" : %d}",
                    tmp_chrbuf, SUNSEN_get_z_pos_temp());
            }
            else if (jtok_tokcmp("z-", &tkns[*t]))
            {
                SUNSEN_face_lux_to_string(tmp_chrbuf, sizeof(tmp_chrbuf),
                                          SUNSEN_FACE_z_neg);
                OBC_IF_printf(
                    "{ \"sunSen\" : \"z-\", \"lux\" : %s, \"temp\" :%d}",
                    tmp_chrbuf, SUNSEN_get_z_neg_temp());
            }
            else
            {
                return JSON_HANDLER_RETVAL_ERROR;
            }
        }
        else
        {
            return JSON_HANDLER_RETVAL_ERROR;
        }
    }
    else
    {
        return JSON_HANDLER_RETVAL_ERROR;
    }
    return t;
}


static json_handler_retval parse_magSen(json_handler_args args)
{
    token_index_t *t = (token_index_t *)args;
    CONFIG_ASSERT(*t < JSON_TKN_CNT);
    *t += 1; /* Advance to first key of json */
    if (jtok_tokcmp("read", &tkns[*t]))
    {
        memset(tmp_chrbuf, 0, sizeof(tmp_chrbuf));
        MAGTOM_measurement_to_string(tmp_chrbuf, sizeof(tmp_chrbuf));
        OBC_IF_printf("{ \"magSen\" : %s}", tmp_chrbuf);
    }
    else if (jtok_tokcmp("reset", &tkns[*t]))
    {
        MAGTOM_reset();
        OBC_IF_printf("{\"magSen\" : \"restarted\"}");
    }
    else
    {
        return JSON_HANDLER_RETVAL_ERROR;
    }

    return t;
}


static json_handler_retval parse_imu(json_handler_args args)
{
    token_index_t *t = (token_index_t *)args;
    CONFIG_ASSERT(*t < JSON_TKN_CNT);
    *t += 1; /* Advance to first key of json */
    if (jtok_tokcmp("read", &tkns[*t]))
    {
#warning NOT IMPLEMENTED YET.
        /** @todo IMPLEMENT */
    }
    else
    {
        return JSON_HANDLER_RETVAL_ERROR;
    }
    return t;
}


static json_handler_retval parse_current(json_handler_args args)
{
    token_index_t *t = (token_index_t *)args;
    CONFIG_ASSERT(*t < JSON_TKN_CNT);
    *t += 1; /* Advance to first key of json */
    if (jtok_tokcmp("rw", &tkns[*t]))
    {
#warning NOT IMPLEMENTED YET.
        /** @todo IMPLEMENT */
    }
    else if (jtok_tokcmp("mqtr", &tkns[*t]))
    {
#warning NOT IMPLEMENTED YET.
        /** @todo IMPLEMENT */
    }
    else
    {
        return JSON_HANDLER_RETVAL_ERROR;
    }
    return t;
}

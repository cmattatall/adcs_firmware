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

#include "jsons.h"

#define JTOK_STANDALONE_TOKENS
#include "jtok.h"

#include "utils.h"

#include "obc_interface.h"

#include "version.h"


#define JSON_TKN_CNT 250

static jtoktok_t     tkns[JSON_TKN_CNT];
static jtok_parser_t parser;


int json_parse(uint8_t *json, uint_least16_t json_len)
{
    CONFIG_ASSERT(json != NULL);

    int json_parse_status = 0;

    int jtok_retval = jtok_parse(&parser, &tkns, JSON_TKN_CNT);
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

        uint_fast16_t t; /* token index */
        t = 0;
        if (jtok_tokcmp("fwVersion", &tkns[t]))
        {
            OBC_printf("{\"fwVersion\" : \"%s\"}%c", FW_VERSION, OBC_MSG_DELIM);
        }
        else if (jtok_tokcmp("hwVersion", &tkns[t]))
        {
            OBC_printf("{\"hwVersion\" : \"%s\"}%c", HW_VERSION, OBC_MSG_DELIM);
        }
        else
        {
        }
    }
    return json_parse_status;
}

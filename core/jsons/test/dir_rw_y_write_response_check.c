/**
 * @file dir_rw_y_write_response_check.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test the response to
 * {"dir_rw_y":"write", "value": >"clock"/"antiClock"<} OBC request
 * @version 0.1
 * @date 2020-12-27
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 * @note
 */

#include <stdio.h>
#include <string.h>

#include "jtok.h"
#include "jsons.h"
#include "json_test_hook.h"

#include "reaction_wheels.h"

#include "version.h"

#define TKN_POOL_SIZE (250u)

static jtok_parser_t p1;
static jtok_tkn_t    tokens1[TKN_POOL_SIZE];

static jtok_tkn_t    tokens2[TKN_POOL_SIZE];
static jtok_parser_t p2;

int main(void)
{
    uint8_t      json[125];
    char         expect[125];
    RW_DIR_t     rw_dirs[2] = {RW_DIR_clockwise, RW_DIR_anticlockwise};
    unsigned int i;
    for (i = 0; i < 2; i++)
    {
        char *dir_str = reacwheel_dir_str(rw_dirs[i]);
        snprintf((char *)json, sizeof(json),
                 "{\"dir_rw_y\":\"write\", \"value\":\"%s\"}", dir_str);
        printf("Testing ADCS response to OBC Request %s ...  ", json);
        int retval = json_parse(json, sizeof(json));
        if (retval != 0)
        {
            printf("Parse of %s failed with status %d\n", json, retval);
            return retval;
        }
        else
        {
            /* Command was processed by JSON module correctly.
             * Now check response against response mandated by design spec */
            snprintf(expect, sizeof(expect), "{\"dir_rw_y\": \"written\" }");
            p1 = jtok_new_parser(expect);

            JTOK_PARSE_STATUS_t status;
            status = jtok_parse(&p1, tokens1, TKN_POOL_SIZE);
            if (status != JTOK_PARSE_STATUS_PARSE_OK)
            {
                printf("Parse of %s failed with status %d\n", expect, status);
                return status;
            }
            else
            {
                p2     = jtok_new_parser((char *)OBC_MESSAGE_SIPHON_BUFFER);
                status = jtok_parse(&p2, tokens2, TKN_POOL_SIZE);
                if (status != JTOK_PARSE_STATUS_PARSE_OK)
                {
                    printf(
                        "Parse of ADCS response : %s failed with status %d\n",
                        OBC_MESSAGE_SIPHON_BUFFER, status);
                    return status;
                }
                else
                {
                    if (jtok_toktokcmp(tokens1, tokens1, tokens2, tokens2))
                    {
                        printf("passed.\n");
                    }
                    else
                    {
                        printf("expected >%s< but received response = >%s<\n",
                               expect, OBC_MESSAGE_SIPHON_BUFFER);
                        printf("failed.\n");
                        return -1;
                    }
                }
            }
        }
    }

    return 0;
}
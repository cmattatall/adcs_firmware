/**
 * @file fw_version_response_check.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test the response to {"fwVersion":"read"} OBC request
 * @version 0.1
 * @date 2020-12-26
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 * @note
 *
 */

#include <stdio.h>
#include <string.h>

#include "jtok.h"
#include "jsons.h"
#include "json_test_hook.h"

#include "version.h"

#define TKN_POOL_SIZE (250u)

static jtok_parser_t p1;
static jtok_tkn_t    tokens1[TKN_POOL_SIZE];

static jtok_tkn_t    tokens2[TKN_POOL_SIZE];
static jtok_parser_t p2;

int main(void)
{
    uint8_t json[] = "{\"fwVersion\":\"read\"}";
    printf("Testing ADCS response to OBC Request %s\n", json);
    int retval = json_parse(json, sizeof(json));
    if (retval != 0)
    {
        return retval;
    }

    char expect[250];
    snprintf(expect, sizeof(expect), "{\"fwVersion\":%s}", FW_VERSION);
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
            printf("Parse of ADCS response : %s failed with status %d\n",
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
                printf("expected >%s< but received response = >%s<\n", expect,
                       OBC_MESSAGE_SIPHON_BUFFER);
                printf("failed.\n");
                return -1;
            }
        }
    }

    return 0;
}
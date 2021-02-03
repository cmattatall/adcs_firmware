#if defined(TARGET_MCU)
#error NATIVE TESTS CANNOT BE RUN ON A BARE METAL MICROCONTROLLER
#endif /* #if defined(TARGET_MCU) */

#include <stdio.h> /* sprintf */
#include <stdlib.h>
#include <limits.h>

#include "jtok.h"
#include "jsons.h"
#include "json_test_hook.h"
#include "magnetorquers.h"

#define TKN_POOL_SIZE (250u)

static jtok_tkn_t tokens1[TKN_POOL_SIZE];
static jtok_tkn_t tokens2[TKN_POOL_SIZE];

int main(void)
{
    uint8_t json[] = "{\"dir_mqtr_x\" : \"read\" }";
    printf("Testing ADCS response to OBC Request %s\n", json);
    int retval = json_parse(json);
    if (retval != 0)
    {
        return retval;
    }

    mqtr_set_wheel_dir(MQTR_x, MQTR_DIR_clockwise);


    char  expect[250];
    char *dir_string = reacwheel_dir_str(MQTR_DIR_clockwise);
    if (dir_string == NULL)
    {
        snprintf(expect, sizeof(expect), "{\"mqtr_rw_x\": \"(NULL)\" }");
    }
    else
    {
        snprintf(expect, sizeof(expect), "{\"mqtr_rw_x\": \"%s\" }", dir_string);
    }


    JTOK_PARSE_STATUS_t status;
    status = jtok_parse(expect, tokens1, TKN_POOL_SIZE);
    if (status != JTOK_PARSE_STATUS_OK)
    {
        printf("Parse of %s failed with status %d\n", expect, status);
        return status;
    }
    else
    {

        status = jtok_parse((char *)OBC_MESSAGE_SIPHON_BUFFER, tokens2,
                            TKN_POOL_SIZE);
        if (status != JTOK_PARSE_STATUS_OK)
        {
            printf("Parse of ADCS response : %s failed with status %d\n",
                   OBC_MESSAGE_SIPHON_BUFFER, status);
            return status;
        }
        else
        {
            if (jtok_toktokcmp(tokens1, tokens2))
            {
                printf("passed. Response was %s\n", OBC_MESSAGE_SIPHON_BUFFER);
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
}
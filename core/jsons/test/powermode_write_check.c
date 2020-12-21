/**
 * @file fw_version_check.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Test to check response to power mode write command from OBC
 * @version 0.1
 * @date 2020-12-21
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 * @note
 */

#if defined(TARGET_MCU)
#error NATIVE TESTS CANNOT BE RUN ON A BARE METAL MICROCONTROLLER
#endif /* #if defined(TARGET_MCU) */

#include <stdio.h> /* sprintf */

#include "jsons.h"
#include "json_test_hook.h"

int main(void)
{
    char *keyValues[] = {"lowPower", "sleep", "blah"};

    char json[250];
    int  i;
    int  retval = 0;
    for (i = 0; i < 3; i++)
    {
        char *       mode = keyValues[i];
        unsigned int json_len =
            sprintf(json, "{\"powerMode\":\"write\", \"value\":\"%s\"}", mode);
        retval = json_parse((uint8_t *)json, json_len);
        if (retval)
        {
            break;
        }
    }
    return retval;
}
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

#include "jsons.h"
#include <assert.h>

#include <stdio.h> /* sprintf */

int main(void)
{
    char *keyValues[] = {"lowPower", "sleep", "blah"};

    char       json[250];
    const char fmt = "{\"powerMode\":\"write\", \"value\":\"%s\"}";
    int        i;
    for (i = 0; i < 3; i++)
    {
        char *       mode     = keyValues[i];
        unsigned int json_len = sprintf(json, fmt, mode);
        int          retval   = json_parse(json, json_len);
        assert(retval == 0);
    }
    return 0;
}
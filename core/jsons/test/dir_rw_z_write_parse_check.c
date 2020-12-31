/**
 * @file dir_rw_z_write_parse_check.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to confirm that incoming json from OBC
 * {"dir_rw_z": "write", "value": "clock"/"antiClock"} gets parsed correctly
 * @version 0.1
 * @date 2020-12-27
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 * @note
 */

#if defined(TARGET_MCU)
#error NATIVE TESTS CANNOT BE RUN ON A BARE METAL MICROCONTROLLER
#endif /* #if defined(TARGET_MCU) */

#include <stdio.h> /* sprintf */
#include <stdlib.h>
#include <limits.h>

#include "jsons.h"
#include "json_test_hook.h"

int main(void)
{
    char *dirs[] = {"clock", "antiClock"};
    char  json[250];
    int   i;
    int   retval = 0;
    for (i = 0; i < 2; i++)
    {
        sprintf(json, "{\"dir_rw_z\": \"write\", \"value\" : \"%s\"}", dirs[i]);
        retval = json_parse((uint8_t *)json);
        if (retval)
        {
            break;
        }
    }
    return retval;
}
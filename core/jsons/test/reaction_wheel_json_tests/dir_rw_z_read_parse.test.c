/**
 * @file dir_rw_z_read_parse_check.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to confirm that incoming json from OBC
 * {"dir_rw_z": "read"} gets parsed correctly
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
    char json[250];
    int  retval = 0;
    sprintf(json, "{\"dir_rw_z\": \"read\"}");
    retval = json_parse((uint8_t *)json);
    return retval;
}
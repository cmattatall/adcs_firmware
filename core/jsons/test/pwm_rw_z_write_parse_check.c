/**
 * @file pwm_rw_z_write_parse_check.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to confirm that incoming json from OBC
 * {"pwm_rw_z": "write", "value" : NUMBER} gets parsed correctly
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
    const unsigned int pwm_max = UINT16_MAX;
    char               json[250];
    int                pwm;
    int                retval = 0;
    for (pwm = 0; pwm < pwm_max; pwm++)
    {
        int json_len =
            sprintf(json, "{\"pwm_rw_z\": \"write\", \"value\" : %d}", pwm);
        retval = json_parse((uint8_t *)json, json_len);
        if (retval)
        {
            break;
        }
    }
    return retval;
}
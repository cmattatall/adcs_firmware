#if defined(TARGET_MCU)
#error NATIVE TESTS CANNOT BE RUN ON A BARE METAL MICROCONTROLLER
#endif /* #if defined(TARGET_MCU) */

#include "jsons.h"
#include <assert.h>

#include <stdio.h> /* sprintf */
#include <stdlib.h>
#include <limits.h>

int main(void)
{
    char fmt[] = "{\"pwm_rw_y\": \"read\"}";

    const unsigned int pwm_max = UINT16_MAX;
    char               json[250];
    int                pwm;
    for (pwm = 0; pwm < pwm_max; pwm++)
    {
        int json_len = sprintf(json, fmt, pwm);
        int retval   = json_parse(json, json_len);
        assert(retval == 0);
    }
    return 0;
}
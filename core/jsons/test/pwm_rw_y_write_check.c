#if defined(TARGET_MCU)
#error NATIVE TESTS CANNOT BE RUN ON A BARE METAL MICROCONTROLLER
#endif /* #if defined(TARGET_MCU) */

#include "jsons.h"
#include "test_hook.h"

#include <assert.h>

#include <stdio.h> /* sprintf */
#include <stdlib.h>
#include <limits.h>

int main(void)
{

    const unsigned int pwm_max = UINT16_MAX;
    char               json[250];
    int                pwm;
    for (pwm = 0; pwm < pwm_max; pwm++)
    {
        int json_len =
            sprintf(json, "{\"pwm_rw_y\": \"write\", \"value\" : %d}", pwm);
        int retval = json_parse((uint8_t *)json, json_len);
        assert(retval == 0);
    }
    return 0;
}
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
        int json_len = sprintf(
            json, "{\"dir_rw_y\": \"write\", \"value\" : \"%s\"}", dirs[i]);
        retval = json_parse((uint8_t *)json, json_len);
        if (retval)
        {
            break;
        }
    }
    return retval;
}
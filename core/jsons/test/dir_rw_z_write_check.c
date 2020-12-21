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
    char *directions[] = {"clock", "antiClock"};
    char  json[250];
    int   i;
    for (i = 0; i < 2; i++)
    {
        int json_len =
            sprintf(json, "{\"dir_rw_z\": \"write\", \"value\" : \"%s\"}",
                    directions[i]);
        int retval = json_parse((uint8_t *)json, json_len);
        assert(retval == 0);
    }
    return 0;
}
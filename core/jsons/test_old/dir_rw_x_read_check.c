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
    char fmt[] = "{\"dir_rw_x\": \"read\"}";
    uint8_t json[250];
    int  i;
    for (i = 0; i < 2; i++)
    {
        int json_len = sprintf(json, fmt);
        int retval   = json_parse(json, json_len);
        assert(retval == 0);
    }
    return 0;
}
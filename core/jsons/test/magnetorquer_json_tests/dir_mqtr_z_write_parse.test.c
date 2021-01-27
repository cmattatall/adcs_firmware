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
    int  retval   = 0;
    sprintf(json, "{\"dir_mqtr_z\": \"write\", \"value\":\"clock\"}");
    retval        = json_parse((uint8_t *)json);
    return retval;
}
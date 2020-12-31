/**
 * @file invalid_json_test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Test module to cover tests of json parsing logic when INVALID
 *        json's are sent to ADCS
 * @version 0.1
 * @date 2020-12-20
 *
 * @copyright Copyright (c) 2020 LORRIS PROJECT DSS
 *
 */
#if defined(TARGET_MCU)
#error NATIVE TESTS CANNOT BE RUN ON A BARE METAL MICROCONTROLLER
#endif /* #if defined(TARGET_MCU) */

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "jsons.h"
#include "json_test_hook.h"

static const char *invalid_jsons[] = {
    "{\"blah\"",
};

int main(void)
{
    int max_i  = sizeof(invalid_jsons) / sizeof(*invalid_jsons);
    int retval = 0;
    for (unsigned int i = 0; i < max_i; i++)
    {
        retval =
            json_parse((uint8_t *)invalid_jsons[i]);
        if (retval == 0)
        {
            break;
        }
    }
    /* we EXPECT these tests to fail.
    If parser treats them as valid it's a problem */
    return !retval;
}
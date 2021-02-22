/**
 * @file mqtr_dir_set_x.test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test magnetorquer API for x direction
 * @version 0.1
 * @date 2021-02-18
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 */

#if defined(TARGET_MCU)
#error NATIVE TESTS CANNOT BE RUN ON A BARE METAL MICROCONTROLLER
#endif /* #if defined(TARGET_MCU) */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

#include "magnetorquers.h"

int main(void)
{
    /* Expect set direction is stored correclty */
    MQTR_DIR_t dirs[2] = {MQTR_DIR_anticlockwise, MQTR_DIR_clockwise};
    uint16_t   i;
    for (i = 0; i < 2; i++)
    {
        char *dir_str = mqtr_dir_str(dirs[i]);
        printf("Testing mqtr API for direction %s ...", dir_str);
        mqtr_set_dir(MQTR_x, dirs[i]);
        if (dirs[i] != mqtr_get_dir(MQTR_x))
        {
            return -1;
        }
    }

    /* Expect full range of values that are not a given direction to
     * return INVALID
     */
    for (i = MQTR_DIR_invalid; i < UINT16_MAX; i++)
    {
        if (MQTR_DIR_invalid != mqtr_set_dir(MQTR_x, i))
        {
            return i;
        }
    }

    return 0;
}
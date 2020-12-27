/**
 * @file reactionwheel_x_dir_set_check.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test reactionwheel interface API functionality for
 * setting/getting reaction wheel X dir value
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

#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

#include "reaction_wheels.h"

int main(void)
{
    /* Expect set direction is stored correclty */
    RW_DIR_t dirs[2] = {RW_DIR_anticlockwise, RW_DIR_clockwise};
    uint16_t i;
    for (i = 0; i < 2; i++)
    {
        char *dir_str = reacwheel_dir_str(dirs[i]);
        printf("Testing reaction wheel API for direction %s ...", dir_str);
        reacwheel_set_wheel_dir(REACTION_WHEEL_x, dirs[i]);
        if (dirs[i] != reacwheel_get_wheel_dir(REACTION_WHEEL_x))
        {
            return -1;
        }
    }

    /* Expect full range of values that are not a given direction to
     * return INVALID
     */
    for (i = RW_DIR_invalid; i < UINT16_MAX; i++)
    {
        if (RW_DIR_invalid != reacwheel_set_wheel_dir(REACTION_WHEEL_x, i))
        {
            return i;
        }
    }

    return 0;
}
#include <stdlib.h>
#include <limits.h>

#include "reaction_wheels.h"

int main(void)
{
    /* Expect set direction is stored correclty */
    RW_DIR_t dirs[2] = {RW_DIR_anticlockwise, RW_DIR_clockwise};
    int      i;
    for (i = 0; i < 2; i++)
    {
        reacwheel_set_wheel_dir(REACTION_WHEEL_y, dirs[i]);
        if (dirs[i] != reacwheel_get_wheel_dir(REACTION_WHEEL_y))
        {
            return -1;
        }
    }

    /* Expect full range of values that are not a given direction to
     * return INVALID
     */
    for (i = RW_DIR_invalid; i < INT_MAX; i++)
    {
        if (RW_DIR_invalid != reacwheel_set_wheel_dir(REACTION_WHEEL_y, i))
        {
            return i;
        }
    }

    return 0;
}
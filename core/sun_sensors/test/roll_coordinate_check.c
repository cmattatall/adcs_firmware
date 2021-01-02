/**
 * @file roll_coordinate_check.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test the roll coordinate system from sun sensor API
 * @version 0.1
 * @date 2021-01-01
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 * @todo
 */

#include <stdio.h>

#include "sun_sensors.h"


int main(void)
{

    sun_vector_t bearing = get_sun_vector();
    if (bearing.roll > 180.f || bearing.roll < 0.0f)
    {
        return -1;
    }
    return 0;
}
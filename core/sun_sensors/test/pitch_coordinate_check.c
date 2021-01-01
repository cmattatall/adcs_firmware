/**
 * @file pitch_coordinate_check.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test the coordinate system for the sun sensor API
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
    if (bearing.pitch > 180.f || bearing.pitch < 0.0f)
    {
        return -1;
    }
    return 0;
}
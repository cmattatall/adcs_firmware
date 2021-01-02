/**
 * @file sun_sensors.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to implement the sun sensor interface
 * @version 0.1
 * @date 2021-01-01
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 * @todo IMPLEMENT THE INTERFACE
 */

#include "sun_sensors.h"

#if defined(TARGET_MCU)
#include "spi.h"
#else
#endif /* #if defined(TARGET_MCU) */


sun_vector_t get_sun_vector(void)
{
    sun_vector_t bearing;
    bearing.pitch = 0.0f;
    bearing.roll  = 0.0f;
    bearing.yaw   = 0.0f;

    return bearing;
}

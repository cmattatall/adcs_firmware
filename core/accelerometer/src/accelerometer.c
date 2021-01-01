/**
 * @file accelerometer.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to provide an interface API to the accellerometer
 * @version 0.1
 * @date 2021-01-01
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 * @todo ACTUALLY IMPLEMENT THE INTERFACE
 */

#if defined(TARGET_MCU)


/** @todo I DON'T KNOW WHAT PHY INTERFACE IS BEING USED TO TALK TO THE
 * ACCELEROMETER */

#include "accelerometer.h"
#else

#endif /* #if defined(TARGET_MCU) */


accelerometer_reading_t accelerometer_get_angular_rate(void)
{
    accelerometer_reading_t rate;
    rate.pitch_rate = 0.0f;
    rate.yaw_rate   = 0.0f;
    rate.roll_rate  = 0.0f;
    return rate;
}

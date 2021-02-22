/**
 * @file imu.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module for the inertial measurement unit API
 * @version 0.1
 * @date 2021-01-02
 * 
 * @copyright Copyright (c) 2021 Carl Mattatall
 * 
 */

#include "imu.h"

#if defined(TARGET_MCU)

#include "bno055.h"

struct bno055_t bno055;

void imu_init(void)
{

}


#else

#endif /* #if defined(TARGET_MCU) */
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
#include <stdio.h>

#include "imu.h"

#if defined(TARGET_MCU)

#include "i2c.h"
#include "bno055.h"

#define I2C_BUFFER_LEN 8
#define I2C0 5
#define BNO055_I2C_BUS_WRITE_ARRAY_INDEX ((u8)1)

static s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
static s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
static void BNO055_delay_msek(u32 msek);
static void IMU_init_i2c(void);


static struct bno055_t bno055;

void IMU_init(void)
{
    IMU_init_i2c();

    bno055.bus_read   = BNO055_I2C_bus_read;
    bno055.bus_write  = BNO055_I2C_bus_write;
    bno055.delay_msec = BNO055_delay_msek;
    bno055.bus_read   = bno055_init(&bno055);
}


static s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    s32 BNO055_iERROR = BNO055_INIT_VALUE;
#if 0
    u8  array[I2C_BUFFER_LEN] = {BNO055_INIT_VALUE};
    u8  stringpos             = BNO055_INIT_VALUE;


    array[BNO055_INIT_VALUE] = reg_addr;

    /* Please take the below API as your reference
     * for read the data using I2C communication
     * add your I2C read API here.
     * "BNO055_iERROR = I2C_WRITE_READ_STRING(DEV_ADDR,
     * ARRAY, ARRAY, 1, CNT)"
     * BNO055_iERROR is an return value of SPI write API
     * Please select your valid return value
     * In the driver BNO055_SUCCESS defined as 0
     * and FAILURE defined as -1
     */
    for (stringpos = BNO055_INIT_VALUE; stringpos < cnt; stringpos++)
    {
        *(reg_data + stringpos) = array[stringpos];
    }
#endif
    return (s8)BNO055_iERROR;
}


s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    s32 BNO055_iERROR = BNO055_INIT_VALUE;

    /* SEE SECTION 4.6 OF DATASHEET */
#if 0
    u8  array[I2C_BUFFER_LEN];
    u8  stringpos = BNO055_INIT_VALUE;

    array[BNO055_INIT_VALUE] = reg_addr;
    for (stringpos = BNO055_INIT_VALUE; stringpos < cnt; stringpos++)
    {
        array[stringpos + BNO055_I2C_BUS_WRITE_ARRAY_INDEX] =
            *(reg_data + stringpos);
    }
#endif
    return (s8)BNO055_iERROR;
}


static void BNO055_delay_msek(u32 msek)
{
    /*Here you can write your own delay routine*/
}
#else

#endif /* #if defined(TARGET_MCU) */


static void IMU_init_i2c(void)
{

#if defined(TARGET_MCU)

/** @todo IMLEMENT */
#warning IMPLEMENT IMU_init_i2c IN TERMS OF THE I2C API DRIVER FUNCTIONS


#else

    printf("Called %s\n", __func__);

#endif /* #if defined(TARGET_MCU) */
}

/**
 * @file magnetorquers.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module for interface to ADCS magnetorquers
 * @version 0.1
 * @date 2020-12-09
 *
 * @copyright Copyright (c) 2020
 *
 */
#include <stdlib.h>
#include <limits.h>

#include "magnetorquers.h"
#include "targets.h"


/** @todo refactor this && reaction wheel table because they're effectively duplicates */
static const char *mqtr_dir_jsonvalue_string[] = {
    [MQTR_DIR_clockwise]     = "clock",
    [MQTR_DIR_anticlockwise] = "antiClock",
    [MQTR_DIR_invalid]       = "invalid",
};


static struct
{
    pwm_t    pwm;
    MQTR_DIR_t dir;
} reaction_wheel_configs[] = {
    [MQTR_x] = {.pwm = PWM_DEFAULT, .dir = MQTR_DIR_clockwise},
    [MQTR_y] = {.pwm = PWM_DEFAULT, .dir = MQTR_DIR_clockwise},
    [MQTR_z] = {.pwm = PWM_DEFAULT, .dir = MQTR_DIR_clockwise},
};

pwm_t mqtr_set_wheel_pwm(MQTR_t wheel, pwm_t value)
{

}


MQTR_DIR_t mqtr_set_wheel_dir(MQTR_t wheel, MQTR_DIR_t dir)
{

}


pwm_t    mqtr_get_wheel_pwm(MQTR_t wheel)
{

}
MQTR_DIR_t mqtr_get_wheel_dir(MQTR_t wheel)
{

}

char *mqtr_dir_str(MQTR_DIR_t dir)
{

}
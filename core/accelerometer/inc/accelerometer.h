#ifndef __ACCELEROMETER_H__
#define __ACCELEROMETER_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

typedef struct
{
    float pitch_rate;
    float yaw_rate;
    float roll_rate;
} accelerometer_reading_t;


/**
 * @brief Get the current angular rate based on the accelerometer reading
 *
 * @return accelerometer_reading_t
 */
accelerometer_reading_t accelerometer_get_angular_rate(void);

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __ACCELEROMETER_H__ */

#ifndef __SUN_SENSORS_H__
#define __SUN_SENSORS_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */


typedef struct
{
    /* Cylindrical coords centered on ideal nadir orientation?? */
    float pitch;
    float yaw;
    float roll;
} sun_vector_t;


/**
 * @brief Get the current satellite sun vector bearing from sun sensors
 *
 * @return sun_vector_t the current satellite bearing
 *
 * @todo NOT IMPLEMENTED YET
 */
sun_vector_t get_sun_vector(void);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __SUN_SENSORS_H__ */

#ifndef __SUN_SENSORS_H__
#define __SUN_SENSORS_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#define NUM_SUN_SENSORS 18

typedef struct
{
    /* Cylindrical coords centered on ideal nadir orientation?? */
    float first;
    float second;
    float third;
} SUNSEN_intensity_t;

typedef struct
{
    SUNSEN_intensity_t vals[NUM_SUN_SENSORS];
} SUNSEN_measurement_t;

/**
 * @brief Measure the intensities of all the photodiodes.
 *
 * @return SUNSEN_measurement_t structure containing all the intensity
 * measurements
 */
SUNSEN_measurement_t SUNSEN_measure_intensities(void);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __SUN_SENSORS_H__ */

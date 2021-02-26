#ifndef __MAGNETOMETER_H__
#define __MAGNETOMETER_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */


typedef struct
{
   int structure_fields_to_change_as_needed;
} MAGTOM_measurement_t;

MAGTOM_measurement_t MAGTOM_get_measurement(void);

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __MAGNETOMETER_H__ */

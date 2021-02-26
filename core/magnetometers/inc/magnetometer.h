#ifndef __MAGNETOMETER_H__
#define __MAGNETOMETER_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

int  MAGTOM_measurement_to_string(char *buf, unsigned int buflen);
void MAGTOM_reset(void);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __MAGNETOMETER_H__ */

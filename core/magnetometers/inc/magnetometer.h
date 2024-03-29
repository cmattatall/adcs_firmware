#ifndef __MAGNETOMETER_H__
#define __MAGNETOMETER_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

void MAGTOM_init(void);
int  MAGTOM_measurement_to_string(char *buf, int buflen);
void MAGTOM_reset(void);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __MAGNETOMETER_H__ */

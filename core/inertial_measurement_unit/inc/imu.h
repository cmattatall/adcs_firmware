#ifndef __IMU_H__
#define __IMU_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */


void IMU_init(void);

int IMU_measurements_to_string(char *buf, unsigned int buflen);

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __IMU_H__ */

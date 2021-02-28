#ifndef __SUN_SENSORS_H__
#define __SUN_SENSORS_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

typedef enum
{
    SUNSEN_FACE_x_pos,
    SUNSEN_FACE_x_neg,
    SUNSEN_FACE_y_pos,
    SUNSEN_FACE_y_neg,
    SUNSEN_FACE_z_pos,
    SUNSEN_FACE_z_neg,
} SUNSEN_FACE_t;

void SUNSEN_PHY_init(void);
int SUNSEN_get_z_pos_temp(void);
int SUNSEN_get_z_neg_temp(void);
int SUNSEN_face_lux_to_string(char *buf, unsigned int len, SUNSEN_FACE_t face);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __SUN_SENSORS_H__ */

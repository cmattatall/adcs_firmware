#ifndef __FAT_ARRAY_H__
#define __FAT_ARRAY_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#include <stdlib.h>

struct fat_array
{
    char * start;
    size_t size;
};

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __FAT_ARRAY_H__ */

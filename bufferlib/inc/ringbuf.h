#ifndef __RINGBUF_H__
#define __RINGBUF_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

/* When RINGBUF_THREADSAFE is defined, functions become thread safe */
// #define RINGBUF_THREADSAFE

#include "fat_array.h"

typedef struct ringbuf_struct *ringbuf_t;


/**
 * @brief ringbuf constructor
 *
 * @param size size of ring buffer
 * @return ringbuf_t buffer handle
 */
ringbuf_t ringbuf_ctor(size_t size);


/**
 * @brief ringbuf destructor
 *
 * @param ringbuf buffer handle
 */
void ringbuf_dtor(ringbuf_t ringbuf);


/**
 * @brief threadsafe read of byte from ring buffer
 *
 * @param ringbuf buffer handle
 * @return data in buffer : address of current byte in ring buffer
 *         no data in buffer : NULL
 *
 * @note reading from the ring buffer advances the outptr
 * @note thread safe
 */
char *ringbuf_read_next(ringbuf_t ringbuf);


/**
 * @brief thread safe write to ring buffer
 *
 * @param ringbuf buffer handle
 * @param byte byte to write
 *
 * @note thread safe
 * @note Will overwrite data
 */
void ringbuf_write_next_byte(ringbuf_t ringbuf, char byte);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __RINGBUF_H__ */

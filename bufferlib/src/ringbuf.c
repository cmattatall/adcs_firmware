/**
 * @file ring_buffer.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Ring buffer module supporting thread safety
 * @version 0.1
 * @date 2020-12-20
 *
 * @copyright Copyright (c) 2020 carl mattatall
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "ringbuf.h"

#if defined(RINGBUF_THREADSAFE)
#include <pthread.h>
#endif /* #if defined(RINGBUF_THREADSAFE) */


struct ringbuf_struct
{
    char *           in_ptr;
    char *           out_ptr;
    size_t           bcnt; /* needed for overlap edgecase handling */
    struct fat_array buf;
#if defined(RINGBUF_THREADSAFE)
    pthread_mutex_t inptr_lock;
    pthread_mutex_t outptr_lock;
    pthread_mutex_t bcnt_lock;
#endif /* #if defined(RINGBUF_THREADSAFE) */
};

/**
 * @brief Set ringbuf's inptr to a given address
 *
 * @param ringbuf buffer handle
 * @param byte_ptr new address for inptr
 *
 * @note Thread safe
 * @note Does nothing if address is outside of buffer range
 */
static void ringbuf_set_inptr_internal(ringbuf_t ringbuf, char *byte_ptr);

/**
 * @brief Set ringbuf's inptr to a given address
 *
 * @param ringbuf buffer handle
 * @param byte_ptr new address for outptr
 *
 * @note Thread safe
 * @note Does nothing if address is outside of buffer range
 */
static void ringbuf_set_outptr_internal(ringbuf_t ringbuf, char *byte_ptr);

/**
 * @brief Read ringbuf's current outptr address
 *
 * @param ringbuf buffer handle
 * @return char* current outptr address
 *
 * @note Thread safe
 */
static char *ringbuf_get_outptr_internal(ringbuf_t ringbuf);

/**
 * @brief Read ringbuf's current inptr address
 *
 * @param ringbuf buffer handle
 * @return char* current inptr address
 *
 * @note Thread safe
 */
static char *ringbuf_get_inptr_internal(ringbuf_t ringbuf);

/**
 * @brief Advance ringbuf's inptr, wrapping if necessary
 *
 * @param ringbuf buffer handle
 */
static void ringbuf_inc_inptr_internal(ringbuf_t ringbuf);

/**
 * @brief Advance ringbuf's outptr, wrapping if necessary and stopping at inptr
 *
 * @param ringbuf buffer handle
 */
static void ringbuf_inc_outptr_internal(ringbuf_t ringbuf);


/**
 * @brief increase ringbuffer byte count
 *
 * @param ringbuf buffer handle
 *
 * @note thread safe
 */
static void ringbuf_inc_bcnt_internal(ringbuf_t ringbuf);


/**
 * @brief decrease ringbuffer byte count
 *
 * @param ringbuf buffer handle
 *
 * @note thread safe
 */
static void ringbuf_dec_bcnt_internal(ringbuf_t ringbuf);


/**
 * @brief peek bytecount in the ring buffer
 *
 * @param ringbuf buffer handle
 * @return size_t buffer byte count
 *
 * @note thread safe
 */
static size_t ringbuf_peek_bcnt_internal(ringbuf_t ringbuf);


/**
 * @brief check if ring buffer is full
 *
 * @param ringbuf buffer handle
 * @return true if full. otherwise false
 *
 * @note thread safe
 */
static bool ringbuf_is_full_internal(ringbuf_t ringbuf);


/**
 * @brief check if ring buffer is empty
 *
 * @param ringbuf buffer handle
 * @return true if empty, otherwise false
 *
 * @note thread safe
 */
static bool ringbuf_is_empty_internal(ringbuf_t ringbuf);


ringbuf_t ringbuf_ctor(size_t size)
{
    ringbuf_t ringbuf = (ringbuf_t)malloc(sizeof(ringbuf));
    assert(ringbuf != NULL);
    ringbuf->buf.start = (char *)malloc(size);
    assert(ringbuf->buf.start != NULL);
    ringbuf->buf.size = size;
    memset(ringbuf->buf.start, 0, size);
    ringbuf->bcnt    = 0;
    ringbuf->in_ptr  = ringbuf->buf.start;
    ringbuf->out_ptr = ringbuf->buf.start;
#if defined(RINGBUF_THREADSAFE)
    pthread_mutex_init(ringbuf->inptr_lock);
    pthread_mutex_init(ringbuf->outptr_lock);
    pthread_mutex_init(ringbuf->bcnt_lock);
#endif /* #if defined(RINGBUF_THREADSAFE) */

    return ringbuf;
}

void ringbuf_dtor(ringbuf_t ringbuf)
{
    if (NULL != ringbuf)
    {
        free(ringbuf->buf.start);
#if defined(RINGBUF_THREADSAFE)
        pthread_mutex_destroy(ringbuf->inptr_lock);
        pthread_mutex_destroy(ringbuf->outptr_lock);
        pthread_mutex_destroy(ringbuf->bcnt_lock);
#endif /* #if defined(RINGBUF_THREADSAFE) */
        free(ringbuf);
    }
}


char *ringbuf_read_next(ringbuf_t ringbuf)
{
    char *current_outptr = ringbuf_get_outptr_internal(ringbuf);
    char *inptr          = ringbuf_get_inptr_internal(ringbuf);

    /* If outptr overlaps inptr, either buffer is full, or buffer is empty */
    if (current_outptr == inptr)
    {
        /*
         * If empty, we have nothing to read,
         * and we should not advance outptr past inptr
         */
        if (ringbuf_is_empty_internal(ringbuf))
        {
            current_outptr = NULL;
        }
    }

    if (current_outptr != NULL)
    {
        ringbuf_inc_outptr_internal(ringbuf);
        ringbuf_dec_bcnt_internal(ringbuf);
    }

    return current_outptr;
}


void ringbuf_write_next_byte(ringbuf_t ringbuf, char byte)
{
    char *inptr = ringbuf_get_inptr_internal(ringbuf);
    *inptr      = byte;
    ringbuf->bcnt += 1;
    ringbuf_inc_bcnt_internal(ringbuf);
    ringbuf_inc_inptr_internal(ringbuf);
}


static void ringbuf_set_inptr_internal(ringbuf_t ringbuf, char *byte_ptr)
{
#if defined(RINGBUF_THREADSAFE)
    pthread_mutex_lock(ringbuf->inptr_lock);
#endif /* #if defined(RINGBUF_THREADSAFE) */

    const char *max_addr = ringbuf->buf.start;
    const char *min_addr = &ringbuf->buf.start[ringbuf->buf.size - 1];
    if (byte_ptr >= min_addr && byte_ptr <= max_addr)
    {
        ringbuf->in_ptr = byte_ptr;
    }
#if defined(RINGBUF_THREADSAFE)
    pthread_mutex_unlock(ringbuf->inptr_lock);
#endif /* #if defined(RINGBUF_THREADSAFE) */
}


static void ringbuf_set_outptr_internal(ringbuf_t ringbuf, char *byte_ptr)
{
#if defined(RINGBUF_THREADSAFE)
    pthread_mutex_lock(ringbuf->outptr_lock);
#endif /* #if defined(RINGBUF_THREADSAFE) */

    const char *max_addr = ringbuf->buf.start;
    const char *min_addr = &ringbuf->buf.start[ringbuf->buf.size - 1];
    if (byte_ptr >= min_addr && byte_ptr <= max_addr)
    {
        ringbuf->out_ptr = byte_ptr;
    }

#if defined(RINGBUF_THREADSAFE)
    pthread_mutex_unlock(ringbuf->outptr_lock);
#endif /* #if defined(RINGBUF_THREADSAFE) */
}


static char *ringbuf_get_outptr_internal(ringbuf_t ringbuf)
{
#if defined(RINGBUF_THREADSAFE)
    pthread_mutex_lock(ringbuf->outptr_lock);
#endif /* #if defined(RINGBUF_THREADSAFE) */

    char *byte_ptr = ringbuf->out_ptr;

#if defined(RINGBUF_THREADSAFE)
    pthread_mutex_unlock(ringbuf->outptr_lock);
#endif /* #if defined(RINGBUF_THREADSAFE) */
    return byte_ptr;
}

static char *ringbuf_get_inptr_internal(ringbuf_t ringbuf)
{
#if defined(RINGBUF_THREADSAFE)
    pthread_mutex_lock(ringbuf->inptr_lock);
#endif /* #if defined(RINGBUF_THREADSAFE) */

    char *byte_ptr = ringbuf->in_ptr;

#if defined(RINGBUF_THREADSAFE)
    pthread_mutex_unlock(ringbuf->inptr_lock);
#endif /* #if defined(RINGBUF_THREADSAFE) */
    return byte_ptr;
}

static void ringbuf_inc_inptr_internal(ringbuf_t ringbuf)
{
    char *inptr = ringbuf_get_inptr_internal(ringbuf);
    if ((inptr - ringbuf->buf.start) == ringbuf->buf.size)
    {
        ringbuf_set_inptr_internal(ringbuf, inptr + 1);
    }
    else
    {
        ringbuf_set_inptr_internal(ringbuf, ringbuf->buf.start);
    }
}


static void ringbuf_inc_outptr_internal(ringbuf_t ringbuf)
{
    char *outptr = ringbuf_get_outptr_internal(ringbuf);
    if ((outptr - ringbuf->buf.start) == ringbuf->buf.size)
    {
        ringbuf_set_outptr_internal(ringbuf, outptr + 1);
    }
    else
    {
        ringbuf_set_outptr_internal(ringbuf, ringbuf->buf.start);
    }
}


static void ringbuf_inc_bcnt_internal(ringbuf_t ringbuf)
{
#if defined(RINGBUF_THREAD_SAFE)
    pthread_mutex_lock(ringbuf->bcnt_lock);
#endif /* #if defined(RINGBUF_THREAD_SAFE) */

    if (ringbuf->bcnt < ringbuf->buf.size)
    {
        ringbuf->bcnt++;
    }

#if defined(RINGBUF_THREAD_SAFE)
    pthread_mutex_unlock(ringbuf->bcnt_lock);
#endif /* #if defined(RINGBUF_THREAD_SAFE) */
}


static void ringbuf_dec_bcnt_internal(ringbuf_t ringbuf)
{
#if defined(RINGBUF_THREAD_SAFE)
    pthread_mutex_lock(ringbuf->bcnt_lock);
#endif /* #if defined(RINGBUF_THREAD_SAFE) */

    if (ringbuf->bcnt > 0)
    {
        ringbuf->bcnt--;
    }

#if defined(RINGBUF_THREAD_SAFE)
    pthread_mutex_unlock(ringbuf->bcnt_lock);
#endif /* #if defined(RINGBUF_THREAD_SAFE) */
}


static size_t ringbuf_peek_bcnt_internal(ringbuf_t ringbuf)
{
    size_t bcnt;
#if defined(RINGBUF_THREAD_SAFE)
    pthread_mutex_lock(ringbuf->bcnt_lock);
#endif /* #if defined(RINGBUF_THREAD_SAFE) */

    bcnt = ringbuf->bcnt;

#if defined(RINGBUF_THREAD_SAFE)
    pthread_mutex_unlock(ringbuf->bcnt_lock);
#endif /* #if defined(RINGBUF_THREAD_SAFE) */

    return bcnt;
}


static bool ringbuf_is_full_internal(ringbuf_t ringbuf)
{
    bool   is_full = false;
    size_t bcnt    = ringbuf_peek_bcnt_internal(ringbuf);
    if (bcnt == ringbuf->buf.size)
    {
        is_full = true;

#if defined(DEBUG)
        char *inptr  = ringbuf_get_inptr_internal(ringbuf);
        char *outptr = ringbuf_get_outptr_internal(ringbuf);
        assert(inptr = outptr);
#endif /* #if defined(DEBUG) */
    }
    return is_full;
}


static bool ringbuf_is_empty_internal(ringbuf_t ringbuf)
{
    bool   is_empty = false;
    size_t bcnt     = ringbuf_peek_bcnt_internal(ringbuf);
    if (bcnt == 0)
    {
        is_empty = true;

#if defined(DEBUG)
        char *inptr  = ringbuf_get_inptr_internal(ringbuf);
        char *outptr = ringbuf_get_outptr_internal(ringbuf);
        assert(inptr = outptr);
#endif /* #if defined(DEBUG) */
    }
    return is_empty;
}

/**
 * @file callback.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module for callback interface as part of injection API
 * @version 0.1
 * @date 2020-12-19
 *
 * @copyright Copyright (c) 2020 LORRIS DISS
 */
#include <stdlib.h>
#include "callback.h"
#include "targets.h"

struct callback_struct_t
{
    callback_args args;
    callback_func func;
};

#if defined(DEBUG)
/* FOR REFERENCE COUNTING */
static void inc_ref(void);
static void dec_ref(void);
#endif /* #if defined(DEBUG) */


static callback_handle_t callback_ctor(void)
{
#if defined(DEBUG)
    inc_ref();
#endif /* #if defined(DEBUG) */
    callback_handle_t cb = (callback_handle_t)malloc(sizeof(*cb));
    CONFIG_ASSERT(cb != NULL); /* make sure allocation succeeded */
    return cb;
}


static void callback_dtor(callback_handle_t cb)
{
    CONFIG_ASSERT(cb != NULL);
#if defined(DEBUG)
    dec_ref();
#endif /* #if defined(DEBUG) */
    free(cb);
}


void callback_exec(callback_handle_t cb)
{
    if (cb->func != NULL)
    {
        cb->func(cb->args);
    }
}


callback_handle_t register_callback(callback_func cb_func,
                                    callback_args cb_args)
{
    callback_handle_t new_cb = callback_ctor();
    CONFIG_ASSERT(NULL != cb_func);
    new_cb->func = cb_func;
    new_cb->args = cb_args;
    return new_cb;
}


void unregister_callback(callback_handle_t cb)
{
    CONFIG_ASSERT(cb != NULL);
    callback_dtor(cb);
}


#if defined(DEBUG)
static unsigned int registered_callback_refcount;

static void inc_ref(void)
{
    registered_callback_refcount++;
}

static void dec_ref(void)
{
    registered_callback_refcount--;
}
#endif /* #if defined(DEBUG) */
#ifndef __CALLBACK_H__
#define __CALLBACK_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

/* clang-format off */
typedef void *callback_args;
typedef void (*callback_func)(callback_args);
/* clang-format on */

typedef struct callback_struct_t *callback_handle_t;

/**
 * @brief register callback
 *
 * @param cb_func callback function to register
 * @param cb_args callback arguments to register
 * @return callback_handle_t opaque ptr to callback
 */
callback_handle_t new_callback(callback_func cb_func, callback_args cb_args);

/**
 * @brief unregister and dealloc a given callback
 *
 * @param cb callback handle to unregister
 *
 * @note REALLY THIS SHOULDN'T BE CALLED AT ALL, SINCE IT USES HEAP.
 *       CALLBACK INSTANTIATION SHOULD OCCUR ONLY DURING FIRMWARE INIT
 */
void unregister_callback(callback_handle_t cb);


/**
 * @brief Execute callback using injection API
 * @param cb callback handle
 */
void callback_exec(callback_handle_t cb);

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __CALLBACK_H__ */

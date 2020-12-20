/**
 * @file uart_interface.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Abstraction layer between PHY peripheral interface and
 * application level protocol
 * @version 0.1
 * @date 2020-12-09
 *
 * @copyright Copyright (c) 2020 DSS - LORIS project
 *
 * @note
 * @todo
 */
#include <stdio.h> /* snprintf */
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include "targets.h"

#include "obc_interface.h"

static uint8_t obcTxBuf[OBC_TX_BUFFER_SIZE];

typedef struct
{
    /*
    void (*init)(void (*rx_func)(uint8_t));
    void (*deinit)(void);
    int (*tx)(uint8_t *, uint_least16_t);
    */
    rx_injector_func init;
    deinit_func      deinit;
    transmit_func    tx;
} OBC_IF_fops;


#if !defined(TARGET_MCU)
static pthread_mutex_t inPtr_lock;
static pthread_mutex_t outPtr_lock;
static pthread_mutex_t rx_flag_lock;
#endif /* if !defined(TARGET_MCU) */

static uint8_t outPtr_read(void);
static void    outPtr_advance(void);
static void    inPtr_write(uint8_t byte);
static uint8_t inPtr_read(void);
static void    inPtr_advance(void);
static void    OBC_IF_receive_byte_internal(uint8_t byte);

static volatile uint8_t  ringbuf[2000];
static volatile uint8_t *inPtr;  /* Read in pointer */
static volatile uint8_t *outPtr; /* Read out pointer */

static OBC_IF_fops ops = {NULL};

static volatile bool OBC_IF_data_received_flag = false;

int OBC_IF_config(rx_injector_func init, deinit_func deinit, transmit_func tx)
{
    int status = 0;
    ops.init   = init;
    ops.deinit = deinit;
    ops.tx     = tx;

    inPtr  = ringbuf;
    outPtr = ringbuf;

    if (ops.init != NULL)
    {
        ops.init(OBC_IF_receive_byte_internal);
    }

    if (ops.tx == NULL)
    {
        status = 1;
    }

#if !defined(TARGET_MCU)
    /*
     * If we're running on native machine, need to emulate OBC
     * using a second thread to read raw IO from a file/terminal/etc
     * as if it were asynchronously receiving bytes from the UART
     *
     * This makes the ring buffer a shared variable and we have to protect it
     * using a mutex.
     *
     * On the ADCS target device, ring buffer is not shared because
     * application always eventually resumes its original context
     * after processing the UART ISR
     * - Carl
     */

    if (pthread_mutex_init(&inPtr_lock, NULL) != 0)
    {
        printf("\n mutex init for inPtr_lock failed\n");
        exit(2);
    }

    if (pthread_mutex_init(&outPtr_lock, NULL) != 0)
    {
        printf("\n mutex init for outPtr_lock failed\n");
        exit(2);
    }

#endif /* #if !defined(TARGET_MCU) */

    return status;
}


void OBC_IF_clear_config(void)
{
    ops.init = NULL;
    ops.tx   = NULL;
    if (ops.deinit != NULL)
    {
        ops.deinit();
        ops.deinit = NULL;
    }

#if !defined(TARGET_MCU)
    pthread_mutex_destroy(&rx_flag_lock);
    pthread_mutex_destroy(&inPtr_lock);
    pthread_mutex_destroy(&outPtr_lock);
#endif /* #if !defined(TARGET_MCU) */
}


#if !defined(TARGET_MCU)
void OBC_IF_receive_byte(uint8_t byte)
{
    OBC_IF_receive_byte_internal(byte);
}
#endif /* #if defined(TARGET_MCU) */

int OCB_IF_get_command_string(uint8_t *buf, uint_least16_t buflen)
{
    CONFIG_ASSERT(buf != NULL);
    int           status      = 0;
    bool          found_delim = false;
    uint_fast16_t i           = 0;
    do
    {
        buf[i] = outPtr_read();
        if (buf[i] == '\0')
        {
            found_delim = true;
        }
        outPtr_advance();
    } while (!found_delim && ++i < buflen);

    if (!found_delim)
    {
        status = 1;
    }
    return status;
}


int OBC_IF_tx(uint8_t *buf, uint_least16_t buflen)
{
    return ops.tx(buf, buflen);
}


bool OBC_IF_dataRxFlag_read(void)
{
    bool flag_state;
#if !defined(TARGET_MCU)
    pthread_mutex_lock(&rx_flag_lock);
#endif /* #if defined(TARGET_MCU) */

    flag_state = OBC_IF_data_received_flag;

#if !defined(TARGET_MCU)
    pthread_mutex_unlock(&rx_flag_lock);
#endif /* #if defined(TARGET_MCU) */
    return flag_state;
}

void OBC_IF_dataRxFlag_write(bool data_state)
{
#if !defined(TARGET_MCU)
    pthread_mutex_lock(&rx_flag_lock);
#endif /* #if defined(TARGET_MCU) */

    OBC_IF_data_received_flag = data_state;

#if !defined(TARGET_MCU)
    pthread_mutex_unlock(&rx_flag_lock);
#endif /* #if defined(TARGET_MCU) */
}


static void OBC_IF_receive_byte_internal(uint8_t byte)
{
    inPtr_write(byte);
    if (inPtr_read() == OBC_MSG_DELIM)
    {
        inPtr_write('\0');
        OBC_IF_dataRxFlag_write(OBC_IF_DATA_RX_FLAG_SET);
    }
    inPtr_advance();
}


static uint8_t outPtr_read(void)
{
#if defined(TARGET_MCU)
    return *outPtr;
#else
    return *outPtr;
#endif /* #if defined(TARGET_MCU) */
}

static void outPtr_advance(void)
{
#if !defined(TARGET_MCU)
    pthread_mutex_lock(&outPtr_lock);
#endif
    if (outPtr == &ringbuf[sizeof(ringbuf) - 1])
    {
        outPtr = ringbuf;
    }
    else
    {
        outPtr++;
    }
#if !defined(TARGET_MCU)
    pthread_mutex_unlock(&outPtr_lock);
#endif /* #if defined(TARGET_MCU) */
}

static void inPtr_write(uint8_t byte)
{
#if !defined(TARGET_MCU)
    pthread_mutex_lock(&inPtr_lock);
#endif
    *inPtr = byte;

#if !defined(TARGET_MCU)
    pthread_mutex_unlock(&inPtr_lock);
#endif /* #if defined(TARGET_MCU) */
}

static uint8_t inPtr_read(void)
{
#if defined(TARGET_MCU)
    return *inPtr;
#else
    return *inPtr;
#endif /* #if defined(TARGET_MCU) */
}


static void inPtr_advance(void)
{
#if !defined(TARGET_MCU)
    pthread_mutex_lock(&inPtr_lock);
#endif
    if (inPtr == &ringbuf[sizeof(ringbuf) - 1])
    {
        inPtr = ringbuf;
    }
    else
    {
        inPtr++;
    }
#if !defined(TARGET_MCU)
    pthread_mutex_unlock(&inPtr_lock);
#endif /* #if defined(TARGET_MCU) */
}

/** @todo THIS FUNCTION IS SO GODDAMN UGLY BUT AT LEAST ITS WORKING - Carl */
int OBC_IF_printf(const char *restrict fmt, ...)
{
    CONFIG_ASSERT(fmt != NULL);
    int     bytes_transmitted = 0;
    va_list args;
    va_start(args, fmt);

#if defined(TARGET_MCU)
    char *fmt_str = (char *)fmt;
#else
    /** @note why the fuck do I even have to add this. I shouldn't have to add
     * it. In fact, according to POSIX spec for termios I shouldn't even have
     * to WORRY about it...
     *
     * The entire reason this is here is so that the god damn newline gets
     * appended to the format string because aparently, POSIX shells can't
     * fucking detect EOL signals consistently...
     *
     * I've spend HOURS finding the problem and then fixing it.
     *
     * IF YOU TOUCH THIS PART I WILL ROLL OVER IN MY GRAVE...
     *
     * - Carl
     */
    char fmt_str[250] = {'\0'};
    CONFIG_ASSERT(strnlen(fmt, sizeof(fmt_str)) <= sizeof(fmt_str));
    strcpy(fmt_str, fmt);
    strcat(fmt_str, "\n");
#endif /* #if defined(TARGET_MCU) */

    memset(obcTxBuf, 0, sizeof(obcTxBuf));
    vsnprintf((char *)obcTxBuf, sizeof(obcTxBuf), fmt_str, args);
    size_t msg_len = strnlen((char *)obcTxBuf, sizeof(obcTxBuf));

    bytes_transmitted = OBC_IF_tx(obcTxBuf, msg_len);
    va_end(args);
    return bytes_transmitted;
}

#if 0
/* Printf wrapper for transmit to make life a bit easier */
#define __OBC_tx_wrapper(fmt, ...)                                             \
    do                                                                         \
    {                                                                          \
        memset(obcTxBuf, 0, sizeof(obcTxBuf));                                 \
        snprintf((char *)obcTxBuf, sizeof(obcTxBuf), fmt "\n", ##__VA_ARGS__); \
        if (++txBufIdx > 1)                                                    \
        {                                                                      \
            txBufIdx = 0;                                                      \
        }                                                                      \
        OBC_IF_tx(obcTxBuf, sizeof(obcTxBuf));                                 \
    } while (0)

#endif
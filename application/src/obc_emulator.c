/**
 * @file obc_emulator.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief source module to emulate behaviour of OBC when building application
 * on host system (independent of target hardware)
 * @version 0.1
 * @date 2020-12-15
 *
 * @copyright Copyright (c) 2020
 *
 * @note
 * @todo
 */

#include "platform.h"

static void *uart_emulator(void *args);
static void  uart_emu_start(void);


static void uart_emu_start(void)
{
    int ret;
    ret = pthread_create(&uart_emu_pthread, NULL, uart_emulator, NULL);
    CONFIG_ASSERT(ret == 0);
}


static void *uart_emulator(void *args)
{
    struct termios new_tio;
    tcgetattr(STDIN_FILENO, &new_tio);
    new_tio.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
    char tmp;
    do
    {
        tmp = getchar();
        if (tmp != EOF)
        {
            uart_receive_byte(tmp);
        }
    } while (tmp != 'q'); /* q for quit */
    return NULL;
}
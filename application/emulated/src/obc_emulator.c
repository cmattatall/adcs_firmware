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

#include "targets.h"

#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>

#include "obc_interface.h"

static void *OBC_EMU(void *args);

static pthread_t OBC_EMU_pthread;


void OBC_EMU_start(void)
{
    int ret;
    ret = pthread_create(&OBC_EMU_pthread, NULL, OBC_EMU, NULL);
    CONFIG_ASSERT(ret == 0);
}

static void *OBC_EMU(void *args)
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
            OBC_IF_receive_byte(tmp);
        }
    } while (tmp != 'q'); /* q for quit */
    return NULL;
}
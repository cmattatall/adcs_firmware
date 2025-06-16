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
 */

#include "targets.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#if defined(linux) || defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <termios.h>
#endif /* defined(linux) || defined(__unix__) || defined(__APPLE__) */


#include "targets.h"
#include "obc_emulator.h"
#include "obc_interface.h"


#define SET_ATTR_NOW TCSANOW

static pthread_t OBC_EMU_pthread;

static void *OBC_EMU(void *args);

int OBC_EMU_tx(uint8_t *buf, uint_least16_t buflen)
{
    CONFIG_ASSERT(buf != NULL);
    char *tmp = malloc(buflen + 1);
    snprintf(tmp, buflen + 1, "%s%c", buf, '\0');
    int bytes_transmitted = printf("%s", tmp);
    free(tmp);
    return bytes_transmitted;
}

void OBC_EMU_start(void)
{
#if defined(linux) || defined(__unix__) || defined(__APPLE__)
    /* Configure terminal to read raw, unbuffered input */
    struct termios new_tio;
    tcgetattr(STDIN_FILENO, &new_tio);
    struct termios old_tio = new_tio;
    new_tio.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, SET_ATTR_NOW, &new_tio);
#elif defined(_WIN32) || defined(WIN32)

#endif /* defined(linux) || defined(__unix__) || defined(__APPLE__) */
    char msg[250];
    sprintf(msg,
            "OBC UART EMULATOR\n"
            "TYPE INTO THE TERMINAL TO SEND RAW BYTES TO OBC INTERFACE\n"
            "PRESS ^C (CTRL + C) TO QUIT\n"
            "CURRENTLY, THE OBC RECEIVE DELIMITER IS >%c<\n",
            OBC_MSG_DELIM);
    OBC_EMU_tx((uint8_t *)msg, (uint_least16_t)sizeof(msg));

    /* Start listener thread */
    int ret;
    ret = pthread_create(&OBC_EMU_pthread, NULL, OBC_EMU, &old_tio);
    CONFIG_ASSERT(ret == 0);
}

static void *OBC_EMU(void *args)
{
    struct termios old_tio = *(struct termios *)args;
    char           tmp;
    do
    {
        tmp = getchar();
        OBC_IF_receive_byte(tmp);
    } while (true);

    /* restore old terminal settings */
    fflush(stdin);
    tcsetattr(STDIN_FILENO, SET_ATTR_NOW, &old_tio);
    return NULL;
}
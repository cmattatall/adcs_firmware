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
#include <stdint.h>
#include <termios.h>

#include "targets.h"
#include "obc_emulator.h"
#include "obc_interface.h"


static void *OBC_EMU(void *args);

static pthread_t OBC_EMU_pthread;

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
    /* Configure terminal to read raw, unbuffered input */
    struct termios new_tio;
    tcgetattr(STDIN_FILENO, &new_tio);
    new_tio.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
    char msg[] = "OBC UART EMULATOR. \nTYPE INTO THE TERMINAL TO SEND RAW "
                 "UNBUFFERED BYTES TO THE OBC INTERFACE MODULE\n";
    OBC_EMU_tx((uint8_t *)msg, (uint_least16_t)sizeof(msg));

    /* Start listener thread */
    int ret;
    ret = pthread_create(&OBC_EMU_pthread, NULL, OBC_EMU, NULL);
    CONFIG_ASSERT(ret == 0);
}

static void *OBC_EMU(void *args)
{
    char tmp;
    do
    {
        tmp = getchar();
        if (tmp != EOF)
        {
            OBC_IF_receive_byte(tmp);
        }

        /*
        if (1 == read(0, &tmp, 1))
        {
            OBC_IF_receive_byte(tmp);
        }
        */
    } while (tmp != 'q'); /* q for quit */

    return NULL;
}

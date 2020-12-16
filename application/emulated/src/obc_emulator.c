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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#if defined(linux) || defined(__unix__) 
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <termios.h>
#elif defined(_WIN32) || defined(WIN32)  
#include <conio.h>

#endif /* defined(linux) || defined(__unix__) */

#include "targets.h"
#include "obc_emulator.h"
#include "obc_interface.h"

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
#if defined(linux) || defined(__unix__) 
    /* Configure terminal to read raw, unbuffered input */
    struct termios new_tio;
    tcgetattr(STDIN_FILENO, &new_tio);
    new_tio.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
    char msg[] = "OBC UART EMULATOR. \nTYPE INTO THE TERMINAL TO SEND RAW "
                 "UNBUFFERED BYTES TO THE OBC INTERFACE MODULE\n";
    OBC_EMU_tx((uint8_t *)msg, (uint_least16_t)sizeof(msg));
#elif defined(_WIN32) || defined(WIN32)  

#endif /* defined(linux) || defined(__unix__) */

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
#if defined(linux) || defined(__unix__) 
        tmp = getchar();
        if (tmp != EOF)
        {
            OBC_IF_receive_byte(tmp);
        }
#elif defined(_WIN32) || defined(WIN32) 
        if (_kbhit()) 
        {
            tmp = _getch();
            OBC_IF_receive_byte(tmp);
        }
#endif /* defined(linux) || defined(__unix__) */

    } while (true);
    return NULL;
}
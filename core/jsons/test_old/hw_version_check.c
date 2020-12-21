/**
 * @file fw_version_check.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Test to check response to hardware version request from OBC
 * @version 0.1
 * @date 2020-12-21
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 * @note
 */

#if defined(TARGET_MCU)
#error NATIVE TESTS CANNOT BE RUN ON A BARE METAL MICROCONTROLLER
#endif /* #if defined(TARGET_MCU) */

#include "jsons.h"
#include <assert.h>

int main(void)
{
    char json = "{\"hwVersion\":\"read\"}";
    int  retval          = json_parse(json, sizeof(json));
    assert(retval == 0);
    return 0;
}
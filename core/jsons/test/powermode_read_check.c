/**
 * @file fw_version_check.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Test to check response to power mode write command from OBC
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
#include "test_hook.h"

#include <assert.h>

#include <stdio.h> /* sprintf */

int main(void)
{
    char         json[250];
    unsigned int json_len = sprintf(json, "{\"powerMode\":\"read\"}");
    int          retval   = json_parse((uint8_t *)json, json_len);
    assert(retval == 0);
    return 0;
}
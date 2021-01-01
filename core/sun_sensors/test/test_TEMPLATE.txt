/**
 * @file example_lib_test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Example test file
 * @version 0.1
 * @date 2020-12-21
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 * @note
 */
#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#include "example_lib_public_header.h"


int main(void)
{
    char         msg[] = "HelloWorld!!\n";
    unsigned int bytes_out;
    bytes_out = example_printf_wrapper(msg, sizeof(msg));
    assert(bytes_out == sizeof(msg));
    return 0;
}

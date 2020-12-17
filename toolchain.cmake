# CMAKE TOOLCHAIN FILE FOR msp430-elf-gcc
# AUTHOR: Carl Mattatall (cmattatall2@gmail.com) 

## USER CAN CHANGE THIS AS NEEDED
set(TOOLCHAIN_PREFIX "msp430-elf")

################################################################################
# DON'T TOUCH PAST THIS POINT 
################################################################################

macro(abort message)
    message(FATAL_ERROR "${message}" "${ARGN}")
endmacro(abort message)

macro(msp430_check_define_macro var)
    if(NOT ${var})
        abort("${${var}} NOT DEFINED. Please define it before invoking ${CMAKE_CURRENT_FUNCTION}")
    endif(NOT ${var})
endmacro(msp430_check_define_macro var)

macro(msp430_check_defines_macro)
    msp430_check_define_macro(MSP430_MCU)
    msp430_check_define_macro(MSP430_MCU_FREQ)
endmacro(msp430_check_defines_macro)

if(MINGW OR CYGWIN OR WIN32)
    set(UTIL_SEARCH_COMMAND where)
elseif(UNIX AND NOT APPLE)
    set(UTIL_SEARCH_COMMAND which)
elseif(APPLE)
    # fairly certain this is going to be "which" but I don't want to assume 
    abort("Apple not supported yet")
else()
    abort("${CMAKE_HOST_SYSTEM_NAME} not supported")
endif()

if(CMAKE_CROSSCOMPILING STREQUAL "ON")
    set(TOOLCHAIN_PREFIX_INTERNAL ${TOOLCHAIN_PREFIX}-)
    set(CMAKE_EXECUTABLE_SUFFIX ".elf")

    set(TOOLCHAIN_GCC_EXE ${TOOLCHAIN_PREFIX_INTERNAL}gcc)
    execute_process(
        COMMAND ${UTIL_SEARCH_COMMAND} ${TOOLCHAIN_GCC_EXE}
        OUTPUT_VARIABLE TOOLCHAIN_GCC_SYMLINK_PATH
        RESULT_VARIABLE TOOLCHAIN_GCC_SYMLINK_NOT_FOUND
        ECHO_OUTPUT_VARIABLE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(TOOLCHAIN_GCC_SYMLINK_NOT_FOUND)
        abort("Could not find ${TOOLCHAIN_GCC_EXE}")
    endif(TOOLCHAIN_GCC_SYMLINK_NOT_FOUND)

    if(MINGW OR CYGWIN OR WIN32)
        
# @todo
        abort("CARL HAS NOT IMPLEMENTED SYMLINK FOLLOW YET")

    elseif(UNIX AND NOT APPLE)
        execute_process(
            COMMAND readlink -f ${TOOLCHAIN_GCC_SYMLINK_PATH}
            OUTPUT_VARIABLE TOOLCHAIN_GCC_TRUE_PATH
            RESULT_VARIABLE TOOLCHAIN_GCC_TRUE_PATH_NOT_FOUND
            ECHO_OUTPUT_VARIABLE
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    elseif(APPLE)
        abort("Apple not supported yet")
    else()
        abort("${CMAKE_HOST_SYSTEM_NAME} not supported")
    endif()

    if(TOOLCHAIN_GCC_TRUE_PATH_NOT_FOUND)
        abort("Could not follow symlink from ${TOOLCHAIN_GCC_SYMLINK_PATH}")
    endif(TOOLCHAIN_GCC_TRUE_PATH_NOT_FOUND)

    # FIND TOOLCHAIN ROOT AND SYSROOT FROM TRUE LOCATION OF GCC
    get_filename_component(TOOLCHAIN_BIN_DIR ${TOOLCHAIN_GCC_TRUE_PATH} DIRECTORY)
    get_filename_component(TOOLCHAIN_ROOT ${TOOLCHAIN_BIN_DIR} DIRECTORY)

    message("TOOLCHAIN_GCC_TRUE_PATH = ${TOOLCHAIN_GCC_TRUE_PATH}")
    message("TOOLCHAIN_GCC_BIN_DIR = ${TOOLCHAIN_GCC_BIN_DIR}")

    message("TOOLCHAIN_ROOT = ${TOOLCHAIN_ROOT}")

    set(CMAKE_SYSROOT "${TOOLCHAIN_ROOT}/${TOOLCHAIN_PREFIX}")


    if(UNIX AND NOT APPLE)
        set(TOOLCHAIN_ROOT "${CCS_PATH}/tools/compiler/msp430-gcc-9.2.0.50_linux64/")
        set(BIN_HINTS "${TOOLCHAIN_ROOT}/bin")
    elseif(WIN32)
        set(TOOLCHAIN_ROOT "${CCS_PATH}\\tools\\compiler\\msp430-gcc-9.2.0.50_win64")
        set(BIN_HINTS "${TOOLCHAIN_ROOT}\\bin")
    elseif(APPLE)
        abort("APPLE SUPPORT HAS NOT BEEN ADDED YET. IT IS ON CARL'S TODO LIST")
    else()
        abort("Unsupported platform ${CMAKE_HOST_SYSTEM_NAME}")
    endif()

    set(CMAKE_SYSTEM_NAME Generic)
    set(CMAKE_SYSTEM_PROCESSOR MSP430)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

    set(CMAKE_SYSROOT ${TOOLCHAIN_ROOT}/${TOOLCHAIN_PREFIX_INTERNAL})
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

    set(MCU_HEADER_DIR "${CCS_PATH}/ccs_base/msp430/include_gcc")
    set(CMAKE_SHARED_FLAGS "-ffunction-sections -fdata-sections")

    set(CMAKE_C_FLAGS_INIT "${CMAKE_SHARED_FLAGS}"
        CACHE INTERNAL "Initial flags for C compiler")

    set(CMAKE_CXX_FLAGS_INIT 
        "${CMAKE_SHARED_FLAGS} -fno-rtti -fno-exceptions" 
        CACHE INTERNAL "Initial flags for C++ compiler")

    set(CMAKE_EXE_LINKER_FLAGS_INIT
        "-Wl,--relax,--gc-sections"
        CACHE INTERNAL "Initial options for linker the VERY first time a CMake build tree is configured")
        
    include(CheckLinkerFlag)



else()
    set(TOOLCHAIN_PREFIX_INTERNAL "")
    if(WIN32 OR CYGWIN OR MINGW)
        set(CMAKE_EXECUTABLE_SUFFIX ".exe")
    elseif(UNIX AND NOT APPLE)
        set(CMAKE_EXECUTABLE_SUFFIX ".out")
    elseif(APPLE)
        abort("Apple is not currently a supported platform")
    else()
        abort("${CMAKE_HOST_SYSTEM_NAME} is not currently a supported platform")
    endif()
endif(CMAKE_CROSSCOMPILING STREQUAL "ON")


set(CMAKE_C_COMPILER_NAME ${TOOLCHAIN_PREFIX_INTERNAL}gcc)
set(CMAKE_ASM_COMPILER_NAME ${TOOLCHAIN_PREFIX_INTERNAL}gcc)
set(CMAKE_CXX_COMPILER_NAME ${TOOLCHAIN_PREFIX_INTERNAL}g++)
set(CMAKE_OBJCOPY_NAME ${TOOLCHAIN_PREFIX_INTERNAL}objcopy)
set(CMAKE_OBJDUMP_NAME ${TOOLCHAIN_PREFIX_INTERNAL}objdump)
set(CMAKE_SIZE_NAME ${TOOLCHAIN_PREFIX_INTERNAL}size)
set(CMAKE_GDB_NAME ${TOOLCHAIN_PREFIX_INTERNAL}gdb) 
find_program(CMAKE_C_COMPILER ${CMAKE_C_COMPILER_NAME} HINTS ${BIN_HINTS} REQUIRED)
find_program(CMAKE_ASM_COMPILER ${CMAKE_ASM_COMPILER_NAME} HINTS ${BIN_HINTS} REQUIRED)
find_program(CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER_NAME} HINTS ${BIN_HINTS} REQUIRED)
find_program(CMAKE_OBJCOPY ${CMAKE_OBJCOPY_NAME} HINTS ${BIN_HINTS} REQUIRED)
find_program(CMAKE_OBJDUMP ${CMAKE_OBJDUMP_NAME} HINTS ${BIN_HINTS} REQUIRED)
find_program(CMAKE_SIZE ${CMAKE_SIZE_NAME} HINTS ${BIN_HINTS} REQUIRED)


function(msp430_add_executable executable)
    msp430_check_defines_macro()
    string(TOUPPER "${MSP430_MCU}" UPPERCASE_MCU_MPN)
    add_executable(${executable})
    target_sources(${executable} PRIVATE ${ARGN})
    target_compile_definitions(${executable} PRIVATE "F_CPU=${MSP430_MCU_FREQ}")
    target_compile_definitions(${executable} PRIVATE "__${UPPERCASE_MCU_MPN}__")
    target_compile_definitions(${executable} PRIVATE "TARGET_MCU")
    target_compile_options(${executable} PRIVATE "-mmcu=${MSP430_MCU}")
    target_include_directories(${executable} PUBLIC "${MCU_HEADER_DIR}")
    target_link_options(${executable} PUBLIC "-Wl,-I${MCU_HEADER_DIR},-L${MCU_HEADER_DIR}")
endfunction(msp430_add_executable executable)

function(msp430_add_library library)
    msp430_check_defines_macro()
    string(TOUPPER "${MSP430_MCU}" UPPERCASE_MCU_MPN)
    add_library(${library})
    target_compile_definitions(${library} PRIVATE "F_CPU=${MSP430_MCU_FREQ}")
    target_compile_definitions(${library} PRIVATE "__${UPPERCASE_MCU_MPN}__")
    target_compile_definitions(${library} PRIVATE "TARGET_MCU")
    target_compile_options(${library} PRIVATE "-mmcu=${MSP430_MCU}")
    target_include_directories(${library} PUBLIC "${MCU_HEADER_DIR}")
    target_link_options(${library} PUBLIC "-Wl,-I${MCU_HEADER_DIR},-L${MCU_HEADER_DIR}")
endfunction(msp430_add_library library)
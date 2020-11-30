# CMAKE TOOLCHAIN FILE FOR msp430-elf-gcc
# AUTHOR: Carl Mattatall (cmattatall2@gmail.com) 

set(CODE_COMPOSER_INSTALL_PATH "/opt/ti/ccs1011/ccs")
set(MSP430_MCU "msp430f5529")
set(MSP430_MCU_FREQ "8000000")

if(NOT CODE_COMPOSER_INSTALL_PATH)
    message(FATAL_ERROR "CODE_COMPOSER_INSTALL_PATH NOT DEFINED. PLEASE DEFINE VIA CLI OR IN TOOLCHAIN FILE")
else()
    set(CCS_PATH "${CODE_COMPOSER_INSTALL_PATH}" CACHE STRING "Installation path for the code composer studio root folder")
endif()


if(NOT DEFINED MSP430_MCU)
    message(FATAL_ERROR "MSP430_MCU NOT DEFINED")
else()
    set(MCU "${MSP430_MCU}" CACHE STRING "MPN of the msp430 microcontroller")
endif()


if(NOT DEFINED MSP430_MCU_FREQ)
    message(FATAL_ERROR "MSP430_MCU_FREQ NOT DEFINED")
else()
    set(MCU_FREQ "${MSP430_MCU_FREQ}" CACHE STRING "Frequency in Hz of the msp430 microcontroller")
endif()

macro(listToOptionString list_string)
    string(REPLACE ";" "\ " list_as_string "${${list_string}}")
    set(${list_string} ${list_as_string})
endmacro(listToOptionString list)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR MSP430)
set(CMAKE_CROSSCOMPILING ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

set(TOOLCHAIN_PREFIX msp430-elf)
set(CMAKE_C_COMPILER_NAME ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_ASM_COMPILER_NAME ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER_NAME ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_OBJCOPY_NAME ${TOOLCHAIN_PREFIX}-objcopy)
set(CMAKE_OBJDUMP_NAME ${TOOLCHAIN_PREFIX}-objdump)
set(CMAKE_SIZE_NAME ${TOOLCHAIN_PREFIX}-size)
set(CMAKE_GDB_NAME ${TOOLCHAIN_PREFIX}-gdb) 

if(UNIX AND NOT APPLE)
    set(BIN_HINTS "${CCS_PATH}/tools/compiler/msp430-gcc-9.2.0.50_linux64/bin/")
elseif(WIN32)
    # @todo
endif()

if(NOT DEFINED BIN_HINTS)
    message(FATAL_ERROR "MSP430 SEARCH PATH HINTS ARE NOT DEFINED. PLEASE DEFINED THEM VIA THE CLI OR IN THE CMAKE TOOLCHAIN FILE FOR YOUR PLATFORM")
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
find_program(CMAKE_C_COMPILER ${CMAKE_C_COMPILER_NAME} HINTS ${BIN_HINTS} REQUIRED)
find_program(CMAKE_ASM_COMPILER ${CMAKE_ASM_COMPILER_NAME} HINTS ${BIN_HINTS} REQUIRED)
find_program(CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER_NAME} HINTS ${BIN_HINTS} REQUIRED)
find_program(CMAKE_OBJCOPY ${CMAKE_OBJCOPY_NAME} HINTS ${BIN_HINTS} REQUIRED)
find_program(CMAKE_OBJDUMP ${CMAKE_OBJDUMP_NAME} HINTS ${BIN_HINTS} REQUIRED)
find_program(CMAKE_SIZE ${CMAKE_SIZE_NAME} HINTS ${BIN_HINTS} REQUIRED)

set(MCU_HEADER_DIR "${CCS_PATH}/ccs_base/msp430/include_gcc")
include_directories(${MCU_HEADER_DIR})

string(TOUPPER "${MCU}" tmp)
add_compile_definitions("__${tmp}__")

set(CMAKE_C_FLAGS_INIT "-ffunction-sections -fdata-sections -mmcu=${MCU} -DF_CPU=${MCU_FREQ} -I"
     CACHE INTERNAL "Initial flags for C compiler")

set(CMAKE_CXX_FLAGS_INIT 
    "-ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -mmcu=${MCU} -DF_CPU=${MCU_FREQ}" 
     CACHE INTERNAL "Initial flags for C++ compiler")

# Second option enables debugging FreeRTOS threads with openOCD + gdb
set(CMAKE_EXE_LINKER_FLAGS_INIT 
    "-Wl, -mmcu=${MCU}" 
    CACHE INTERNAL "Initial options for linker.")
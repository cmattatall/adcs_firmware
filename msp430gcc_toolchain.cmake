# CMAKE TOOLCHAIN FILE FOR msp430-elf-gcc
# AUTHOR: Carl Mattatall (cmattatall2@gmail.com) 


# @todo THIS PART NEEDS TO BE REFACTORED TO ACTUALLY SEARCH FOR THE DIRECTORIES
if(WIN32)
    set(MCU_HEADER_DIR "C:\\msp430_toolchain\\msp430-gcc-support-files\\include\\")
elseif(UNIX AND NOT APPLE)
     set(MCU_HEADER_DIR "/opt/msp430_toolchain/msp430-gcc-support-files/include")
elseif(APPLE)
    message(FATAL_ERROR "Apple not supported yet")
else()
    message(FATAL_ERROR "${CMAKE_HOST_SYSTEM_NAME} not supported")
endif()


# this is a shitty hack to pass -msp430_mcu flag 
# to CMakeDetermineCompilerABI.cmake
# because sadly, msp430-elf-gcc NEEDS -mmcu flag to compile...
#
# For whatever reason, the morons at TI don't allow assumption of a default
# or base ISA...
if(MSP430_MCU)
    set(ENV{MSP430_MCU} ${MSP430_MCU})
endif(MSP430_MCU)

if(MINGW OR CYGWIN OR WIN32)
    set(UTIL_SEARCH_COMMAND where)
elseif(UNIX AND NOT APPLE)
    set(UTIL_SEARCH_COMMAND which)
elseif(APPLE)
    set(UTIL_SEARCH_COMMAND which)
else()
    message(FATAL_ERROR "SYSTEM : ${CMAKE_HOST_SYSTEM_NAME} not supported")
endif()

set(TOOLCHAIN_PREFIX "msp430-elf")
set(CMAKE_STATIC_LIBRARY_SUFFIX_C ".statlib")

set(TOOLCHAIN_GCC_EXE ${TOOLCHAIN_PREFIX}-gcc)
execute_process(
    COMMAND ${UTIL_SEARCH_COMMAND} ${TOOLCHAIN_GCC_EXE}
    OUTPUT_VARIABLE TOOLCHAIN_GCC_SYMLINK_PATH
    RESULT_VARIABLE TOOLCHAIN_GCC_SYMLINK_NOT_FOUND
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(TOOLCHAIN_GCC_SYMLINK_NOT_FOUND)
    abort("Could not find ${TOOLCHAIN_GCC_EXE}")
endif(TOOLCHAIN_GCC_SYMLINK_NOT_FOUND)

if(MINGW OR CYGWIN OR WIN32)
    set(TOOLCHAIN_GCC_TRUE_PATH ${TOOLCHAIN_GCC_SYMLINK_PATH})
elseif(UNIX AND NOT APPLE)
    execute_process(
        COMMAND readlink -f ${TOOLCHAIN_GCC_SYMLINK_PATH}
        OUTPUT_VARIABLE TOOLCHAIN_GCC_TRUE_PATH
        RESULT_VARIABLE TOOLCHAIN_GCC_TRUE_PATH_NOT_FOUND
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
elseif(APPLE)
    abort("Apple not supported yet")
else()
    abort("${CMAKE_HOST_SYSTEM_NAME} not supported")
endif()

if(NOT TOOLCHAIN_GCC_TRUE_PATH)
    abort("TOOLCHAIN_GCC_TRUE_PATH not defined. Something went wrong during toolchain configuration")
endif(NOT TOOLCHAIN_GCC_TRUE_PATH)

if(TOOLCHAIN_GCC_TRUE_PATH_NOT_FOUND)
    abort("Could not follow symlink from ${TOOLCHAIN_GCC_SYMLINK_PATH}")
endif(TOOLCHAIN_GCC_TRUE_PATH_NOT_FOUND)

get_filename_component(TOOLCHAIN_BIN_DIR ${TOOLCHAIN_GCC_TRUE_PATH} DIRECTORY)
get_filename_component(TOOLCHAIN_ROOT ${TOOLCHAIN_BIN_DIR} DIRECTORY)
list(APPEND BINUTILS_SEARCH_HINTS "${TOOLCHAIN_BIN_DIR}")

# CROSS COMPILE CONFIG
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR MSP430)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# SEARCH OPTIONS
set(CMAKE_SYSROOT "${TOOLCHAIN_ROOT}/${TOOLCHAIN_PREFIX}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# BINUTILS NAMES
set(CMAKE_C_COMPILER_NAME ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_ASM_COMPILER_NAME ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER_NAME ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_OBJCOPY_NAME ${TOOLCHAIN_PREFIX}-objcopy)
set(CMAKE_OBJDUMP_NAME ${TOOLCHAIN_PREFIX}-objdump)
set(CMAKE_SIZE_NAME ${TOOLCHAIN_PREFIX}-size)
set(CMAKE_GDB_NAME ${TOOLCHAIN_PREFIX}-gdb) 

find_program(
    CMAKE_C_COMPILER 
    NAMES ${CMAKE_C_COMPILER_NAME}
    HINTS ${BINUTILS_SEARCH_HINTS}
    REQUIRED
)

find_program(
    CMAKE_ASM_COMPILER 
    ${CMAKE_ASM_COMPILER_NAME}
    HINTS ${BINUTILS_SEARCH_HINTS} 
    REQUIRED
)

find_program(
    CMAKE_CXX_COMPILER 
    NAMES ${CMAKE_CXX_COMPILER_NAME}
    HINTS ${BINUTILS_SEARCH_HINTS} 
    REQUIRED
)

find_program(
    CMAKE_OBJCOPY 
    NAMES ${CMAKE_OBJCOPY_NAME}
    HINTS ${BINUTILS_SEARCH_HINTS} 
    REQUIRED
)

find_program(
    CMAKE_OBJDUMP 
    NAMES ${CMAKE_OBJDUMP_NAME}
    HINTS ${BINUTILS_SEARCH_HINTS} 
    REQUIRED
)

find_program(
    CMAKE_SIZE 
    NAMES ${CMAKE_SIZE_NAME}
    HINTS ${BINUTILS_SEARCH_HINTS} 
    REQUIRED
)

find_program(
    CMAKE_GDB 
    NAMES ${CMAKE_GDB_NAME}
    HINTS ${BINUTILS_SEARCH_HINTS} 
    REQUIRED
)


set(CMAKE_C_FLAGS_INIT "-ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS_INIT "-ffunction-sections -fdata-sections -fno-rtti -fno-exceptions")

set(CMAKE_C_FLAGS_DEBUG "-Wall -Wshadow -Wextra -O0 -g -ggdb -DDEBUG" CACHE INTERNAL "")
set(CMAKE_C_FLAGS_RELEASE "-Wall -O3 -DNDEBUG")

# so aparently, even code composer studio was broken with the switch to gcc 9.2 backend in 2016
# see https://e2e.ti.com/support/tools/ccs/f/81/t/504524?Linker-error-with-latest-msp430-gcc
#
# current workaround is to force gcc to link against the correct multiplication lib
# code composer is doing this same thing behind the scenes but it's a shame that the maintainers of 
# the msp430 gcc port STILL haven't fixed the issue after 4 years of active work on it...
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,--relax,--gc-sections,--undefined=__mspabi_mpyi -lmul_f5")

include(CheckLinkerFlag)

if(NOT COMMAND _add_executable)
function(add_executable executable)

    if(NOT MSP430_MCU)
        if(DEFINED ENV{MSP430_MCU}) # check environment
            set(MSP430_MCU $ENV{MSP430_MCU})
        else()
            message(FATAL_ERROR "MSP430_MCU NOT DEFINED IN ${CMAKE_CURRENT_LIST_FILE} OR IN ENVIRONMENT.")
        endif(DEFINED ENV{MSP430_MCU})
    endif(NOT MSP430_MCU)

    set(${executable}_LINKER_SCRIPT "${MSP430_MCU}.ld")
    
    string(TOUPPER "${MSP430_MCU}" UPPERCASE_MCU_MPN)
    _add_executable(${executable} ${ARGN})

    set(executable_output_name "${executable}.elf")
    set_target_properties(${executable} PROPERTIES OUTPUT_NAME ${executable_output_name})

    string(TOUPPER ${MSP430_MCU} MSP430_MCU_DEFINE)
    target_compile_definitions(${executable} PRIVATE "__${MSP430_MCU_DEFINE}__")
    target_compile_definitions(${executable} PRIVATE "TARGET_MCU")
    target_compile_options(${executable} PRIVATE "-mmcu=${MSP430_MCU}")
    target_include_directories(${executable} PUBLIC "${MCU_HEADER_DIR}")
    target_link_options(${executable} PUBLIC "-Wl,-I${MCU_HEADER_DIR},-L${MCU_HEADER_DIR},-T,${${executable}_LINKER_SCRIPT}")

    add_custom_target(${executable}_postbuild ALL DEPENDS ${executable})
    add_custom_command( 
        TARGET ${executable}_postbuild
        POST_BUILD
        DEPENDS ${executable}
        COMMENT "Built executable ${elf_file} with the following size:"
        COMMAND ${CMAKE_SIZE} -B "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${executable_output_name}"
    )

    add_custom_command( 
        TARGET ${executable}_postbuild
        POST_BUILD
        DEPENDS ${executable}
        COMMENT "Producing a hex output using ${CMAKE_OBJCOPY}"
        COMMAND ${CMAKE_OBJCOPY} -O ihex -I elf32-little "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${executable_output_name}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${executable}.hex"
    )

    add_custom_command( 
        TARGET ${executable}_postbuild
        POST_BUILD
        DEPENDS ${executable}
        COMMENT "Producing a binary output using ${CMAKE_OBJCOPY}"
        COMMAND ${CMAKE_OBJCOPY} -O binary -I elf32-little "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${executable_output_name}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${executable}.bin"
    )

    add_custom_command( 
        TARGET ${executable}_postbuild
        POST_BUILD
        DEPENDS ${executable}
        COMMENT "Generating lss file from ${elf_file} using ${CMAKE_OBJDUMP}"
        COMMAND ${CMAKE_OBJDUMP} -xh "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${executable_output_name}" > "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${executable}.lss"
    )

endfunction(add_executable executable)
endif(NOT COMMAND _add_executable)


if(NOT COMMAND _add_library)
function(add_library library)
        
    if(NOT MSP430_MCU)
        if(DEFINED ENV{MSP430_MCU}) # check environment
            set(MSP430_MCU $ENV{MSP430_MCU})
        else()
            message(FATAL_ERROR "MSP430_MCU NOT DEFINED IN ${CMAKE_CURRENT_LIST_FILE} OR IN ENVIRONMENT.")
        endif(DEFINED ENV{MSP430_MCU})
    endif(NOT MSP430_MCU)

    set(${library}_LINKER_SCRIPT "${MSP430_MCU}.ld")

    string(TOUPPER "${MSP430_MCU}" UPPERCASE_MCU_MPN)
    _add_library(${library} ${ARGN})

    string(TOUPPER ${MSP430_MCU} MSP430_MCU_DEFINE)
    target_compile_definitions(${library} PRIVATE "__${MSP430_MCU_DEFINE}__")
    target_compile_definitions(${library} PRIVATE "TARGET_MCU")
    target_compile_options(${library} PRIVATE "-mmcu=${MSP430_MCU}")
    target_include_directories(${library} PUBLIC "${MCU_HEADER_DIR}")
    target_link_options(${library} PUBLIC "-Wl,-I${MCU_HEADER_DIR},-L${MCU_HEADER_DIR},-T,${${library}_LINKER_SCRIPT}")

endfunction(add_library library)
endif(NOT COMMAND _add_library)




# CMAKE TOOLCHAIN FILE FOR msp430-elf-gcc
# AUTHOR: Carl Mattatall (cmattatall2@gmail.com) 

## USER CAN CHANGE THIS AS NEEDED


# So basically, this part right here, I'm relying on code composer to provide msp430.h 
# because I'm sick and tired of trying to find an open source download
# for the msp430xxxx.h device header files
# vendor lock in is stupid and I swear I will never willingly choose to use
# TI chipsets for the rest of my career if I can avoid it.
# - Carl
if(WIN32)
    set(CODE_COMPOSER_INSTALL_PATH "C:\\ti\\ccs1011\\ccs")
elseif(UNIX AND NOT APPLE)
    set(CODE_COMPOSER_INSTALL_PATH "/opt/ti/ccs1011/ccs")
elseif(APPLE)
    message(FATAL_ERROR "Apple not supported")
else()
    message(FATAL_ERROR "${CMAKE_HOST_SYSTEM_NAME} not supported")
endif()

################################################################################
# DON'T TOUCH PAST THIS POINT 
################################################################################
set(MCU_HEADER_DIR "${CODE_COMPOSER_INSTALL_PATH}/ccs_base/msp430/include_gcc")

macro(abort message)
    message(FATAL_ERROR "${message}" "${ARGN}")
endmacro(abort message)

macro(msp430_check_define_macro var)
    if(NOT ${var})
        abort("[${CMAKE_CURRENT_LIST_FILE}] ${var} NOT DEFINED. Please define it before invoking ${CMAKE_CURRENT_FUNCTION}")
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
    # fairly certain this is going to be "$which" but I don't want to assume 
    abort("Apple not supported yet")
else()
    abort("${CMAKE_HOST_SYSTEM_NAME} not supported")
endif()

if(CMAKE_CROSSCOMPILING)
    set(TOOLCHAIN_PREFIX "msp430-elf")
    set(TOOLCHAIN_PREFIX_INTERNAL "${TOOLCHAIN_PREFIX}-")
    set(EXECUTABLE_SUFFIX ".elf")

    set(TOOLCHAIN_GCC_EXE ${TOOLCHAIN_PREFIX_INTERNAL}gcc)
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
        # for now, since following shortcuts / aliased targets in windows
        # is something I wouldn't wish on my worse enemy, we will just 
        # require that the executable is directly in PATH and not aliased
        # in PATH...
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

    # FIND TOOLCHAIN ROOT AND SYSROOT FROM TRUE LOCATION OF GCC
    get_filename_component(TOOLCHAIN_BIN_DIR ${TOOLCHAIN_GCC_TRUE_PATH} DIRECTORY)
    get_filename_component(TOOLCHAIN_ROOT ${TOOLCHAIN_BIN_DIR} DIRECTORY)
    list(APPEND BINUTILS_SEARCH_HINTS "${TOOLCHAIN_BIN_DIR}")

    set(CMAKE_SYSTEM_NAME Generic)
    set(CMAKE_SYSTEM_PROCESSOR MSP430)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

    set(CMAKE_SYSROOT "${TOOLCHAIN_ROOT}/${TOOLCHAIN_PREFIX}")
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

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
    set(TOOLCHAIN_PREFIX "")
    if(WIN32 OR CYGWIN OR MINGW)
        set(EXECUTABLE_SUFFIX ".exe")
    elseif(UNIX AND NOT APPLE)
        set(EXECUTABLE_SUFFIX ".out")
    elseif(APPLE)
        set(EXECUTABLE_SUFFIX ".out")
    else()
        abort("${CMAKE_HOST_SYSTEM_NAME} is not currently a supported platform")
    endif()
endif(CMAKE_CROSSCOMPILING)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} CACHE INTERNAL "")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} CACHE INTERNAL "")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} CACHE INTERNAL "")

if(NOT EXECUTABLE_SUFFIX)
    abort("EXECUTABLE_SUFFIX IS NOT DEFINED!")
else()
    set(CMAKE_EXECUTABLE_SUFFIX "${EXECUTABLE_SUFFIX}" CACHE INTERNAL "")
    mark_as_advanced(FORCE CMAKE_EXECUTABLE_SUFFIX)
endif(NOT EXECUTABLE_SUFFIX)


set(CMAKE_C_COMPILER_NAME ${TOOLCHAIN_PREFIX_INTERNAL}gcc)
set(CMAKE_ASM_COMPILER_NAME ${TOOLCHAIN_PREFIX_INTERNAL}gcc)
set(CMAKE_CXX_COMPILER_NAME ${TOOLCHAIN_PREFIX_INTERNAL}g++)
set(CMAKE_OBJCOPY_NAME ${TOOLCHAIN_PREFIX_INTERNAL}objcopy)
set(CMAKE_OBJDUMP_NAME ${TOOLCHAIN_PREFIX_INTERNAL}objdump)
set(CMAKE_SIZE_NAME ${TOOLCHAIN_PREFIX_INTERNAL}size)
set(CMAKE_GDB_NAME ${TOOLCHAIN_PREFIX_INTERNAL}gdb) 


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


###############################################################################
# END SCRIPT, START EXPORTED FUNCTIONS
###############################################################################


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

    set_target_properties(
        ${executable}
        PROPERTIES
        SUFFIX "$CACHE{CMAKE_EXECUTABLE_SUFFIX}"
    )

    add_custom_target(${executable}_postbuild ALL DEPENDS ${executable})
    add_custom_command( 
        TARGET ${executable}_postbuild
        POST_BUILD
        DEPENDS ${executable}
        COMMENT "Built executable ${elf_file} with the following size:"
        COMMAND ${CMAKE_SIZE} -B "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${executable}$CACHE{CMAKE_EXECUTABLE_SUFFIX}"
    )

    add_custom_command( 
        TARGET ${executable}_postbuild
        POST_BUILD
        DEPENDS ${executable}
        COMMENT "Producing a hex output using ${CMAKE_OBJCOPY}"
        COMMAND ${CMAKE_OBJCOPY} -O ihex -I elf32-little "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${executable}$CACHE{CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${executable}.hex"
    )

    add_custom_command( 
        TARGET ${executable}_postbuild
        POST_BUILD
        DEPENDS ${executable}
        COMMENT "Producing a binary output using ${CMAKE_OBJCOPY}"
        COMMAND ${CMAKE_OBJCOPY} -O binary -I elf32-little "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${executable}$CACHE{CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${executable}.bin"
    )

    add_custom_command( 
        TARGET ${executable}_postbuild
        POST_BUILD
        DEPENDS ${executable}
        COMMENT "Generating lss file from ${elf_file} using ${CMAKE_OBJDUMP}"
        COMMAND ${CMAKE_OBJDUMP} -x "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${executable}$CACHE{CMAKE_EXECUTABLE_SUFFIX}" > "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${executable}.lss"
    )

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
    #set_target_properties(
    #    ${library}
    #    PROPERTIES
    #    # PROPERTIES GO HERE
    #)
endfunction(msp430_add_library library)

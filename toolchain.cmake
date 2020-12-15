# CMAKE TOOLCHAIN FILE FOR msp430-elf-gcc
# AUTHOR: Carl Mattatall (cmattatall2@gmail.com) 

if(CMAKE_CROSSCOMPILING STREQUAL "ON")
    if(WIN32)
    #@todo FIGURE OUT WHERE IT INSTALLS    
        message(FATAL_ERROR "WINDOWS PATH NOT SUPPORTED YET")
    elseif(UNIX AND NOT APPLE)
        set(CODE_COMPOSER_INSTALL_PATH "/opt/ti/ccs1011/ccs")
    elseif(APPLE)
        message(FATAL_ERROR "Apple not supported")
    else()
        message(FATAL_ERROR "${CMAKE_HOST_SYSTEM_NAME} not supported")
    endif()
    set(TOOLCHAIN_PREFIX msp430-elf-)

    macro(msp430_check_define_macro var)
        if(NOT ${var})
            message(FATAL_ERROR "${${var}} NOT DEFINED. Please define it before invoking ${CMAKE_CURRENT_FUNCTION}")
        endif(NOT ${var})
    endmacro(msp430_check_define_macro var)

    macro(msp430_check_defines_macro)
        msp430_check_define_macro(MSP430_MCU)
        msp430_check_define_macro(MSP430_MCU_FREQ)
    endmacro(msp430_check_defines_macro)

    if(NOT CODE_COMPOSER_INSTALL_PATH)
        message(FATAL_ERROR "CODE_COMPOSER_INSTALL_PATH NOT DEFINED. PLEASE DEFINE VIA CLI OR IN TOOLCHAIN FILE")
    else()
        set(CCS_PATH "${CODE_COMPOSER_INSTALL_PATH}" CACHE STRING "Installation path for the code composer studio root folder")
    endif()

    if(UNIX AND NOT APPLE)
        set(TOOLCHAIN_ROOT "${CCS_PATH}/tools/compiler/msp430-gcc-9.2.0.50_linux64/")
        set(BIN_HINTS "${TOOLCHAIN_ROOT}/bin")
    elseif(WIN32)
        message(FATAL_ERROR "WIN32 SUPPORT HAS NOT BEEN ADDED YET. IT IS ON CARL'S TODO LIST")
    elseif(APPLE)
        message(FATAL_ERROR "APPLE SUPPORT HAS NOT BEEN ADDED YET. IT IS ON CARL'S TODO LIST")
    else()
        message(FATAL_ERROR "Unsupported platform ${CMAKE_HOST_SYSTEM_NAME}")
    endif()

    if(NOT DEFINED BIN_HINTS)
        message(FATAL_ERROR "MSP430 SEARCH PATH HINTS ARE NOT DEFINED. PLEASE DEFINED THEM VIA THE CLI OR IN THE CMAKE TOOLCHAIN FILE FOR YOUR PLATFORM")
    endif()

    set(CMAKE_SYSTEM_NAME Generic)
    set(CMAKE_SYSTEM_PROCESSOR MSP430)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

    set(CMAKE_SYSROOT ${TOOLCHAIN_ROOT}/${TOOLCHAIN_PREFIX})
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

else()
    # empty prefix so it uses "gcc" instead of "msp430-elf-gcc"
    set(TOOLCHAIN_PREFIX "") 
endif(CMAKE_CROSSCOMPILING STREQUAL "ON")


set(CMAKE_C_COMPILER_NAME ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER_NAME ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER_NAME ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_OBJCOPY_NAME ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_OBJDUMP_NAME ${TOOLCHAIN_PREFIX}objdump)
set(CMAKE_SIZE_NAME ${TOOLCHAIN_PREFIX}size)
set(CMAKE_GDB_NAME ${TOOLCHAIN_PREFIX}gdb) 
find_program(CMAKE_C_COMPILER ${CMAKE_C_COMPILER_NAME} HINTS ${BIN_HINTS} REQUIRED)
find_program(CMAKE_ASM_COMPILER ${CMAKE_ASM_COMPILER_NAME} HINTS ${BIN_HINTS} REQUIRED)
find_program(CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER_NAME} HINTS ${BIN_HINTS} REQUIRED)
find_program(CMAKE_OBJCOPY ${CMAKE_OBJCOPY_NAME} HINTS ${BIN_HINTS} REQUIRED)
find_program(CMAKE_OBJDUMP ${CMAKE_OBJDUMP_NAME} HINTS ${BIN_HINTS} REQUIRED)
find_program(CMAKE_SIZE ${CMAKE_SIZE_NAME} HINTS ${BIN_HINTS} REQUIRED)
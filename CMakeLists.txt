cmake_minimum_required(VERSION 3.16)
################################################################################
##              USER CONFIGURED 
################################################################################
# - when cross compiling is off, we build to run on our host system
# 
# - when cross compiling is on, we build for target device
################################################################################
set(CMAKE_CROSSCOMPILING ON)

set(MCU_MPN "msp430f5529") #set this as a lowercase string of your part number
set(MCU_FREQ "8000000")    #set this as the HSE input freq in Hz

################################################################################
project(ADCS_firmware LANGUAGES C ASM)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options("-Wall")
    add_compile_options("-Wshadow")
endif()


function(ADCS_add_executable target)
    if(CMAKE_CROSSCOMPILING STREQUAL "ON")
        set(MSP430_MCU ${MCU_MPN})
        set(MSP430_MCU_FREQ ${MCU_FREQ})
        msp430_add_executable(${target} ${ARGN})
    else()
        add_executable(${target} ${ARGN})
    endif(CMAKE_CROSSCOMPILING STREQUAL "ON")
endfunction(ADCS_add_executable target)


function(ADCS_add_library target)
    if(CMAKE_CROSSCOMPILING STREQUAL "ON")
        set(MSP430_MCU ${MCU_MPN})
        set(MSP430_MCU_FREQ ${MCU_FREQ})
        msp430_add_library(${target} ${ARGN})
    else()
        add_library(${target} ${ARGN})
    endif(CMAKE_CROSSCOMPILING STREQUAL "ON")
endfunction(ADCS_add_library target)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/shared/inc")

# for some reason, you have to manually run this command. I haven't got it working yet
# FOR THE FIRST TIME, DO 
# $git submodule update --force --remote --recursive --init
# NOTICE THE --init on the end.
execute_process(COMMAND "git submodule update --force --remote --recursive")
add_subdirectory(JTOK)
add_subdirectory(drivers)
add_subdirectory(application)
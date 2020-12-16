cmake_minimum_required(VERSION 3.16)
################################################################################
##              USER CONFIGURED 
################################################################################
# - when cross compiling is off, we build to run on our host system
# 
# - when cross compiling is on, we build for target device
################################################################################


# https://sourceforge.net/projects/mingw-w64/

if(NOT DEFINED CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "CMAKE_CROSSCOMPILING IS NOT DEFINED. Please define it as \"ON\" or \"OFF\"")
    return()
endif()

set(MCU_MPN "msp430f5529") #set this as a lowercase string of your part number
set(MCU_FREQ "8000000")    #set this as the HSE input freq in Hz

################################################################################
project(ADCS_firmware LANGUAGES C ASM)
message("") 
message("PROJECT : ${CMAKE_PROJECT_NAME}")
message("TARGET : ${CMAKE_SYSTEM_NAME}")
message("PROCESSOR : ${CMAKE_SYSTEM_PROCESSOR}")
message("") 

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options("-Wall")
    add_compile_options("-Wshadow")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(CMAKE_CROSSCOMPILING STREQUAL "ON")
    set(CMAKE_EXECUTABLE_SUFFIX ".elf")
else()
    if(WIN32)
        set(CMAKE_EXECUTABLE_SUFFIX ".exe")
    elseif(UNIX)
        set(CMAKE_EXECUTABLE_SUFFIX ".out")
    else()
        message("UNSUPPORTED PLATFORM : ${CMAKE_HOST_SYSTEM_NAME}")
    endif()
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

add_compile_options("-O0")

# for some reason, you have to manually run this command. I haven't got it working yet
# FOR THE FIRST TIME, DO 
# $git submodule update --force --remote --recursive --init
# NOTICE THE --init on the end.
execute_process(COMMAND "git submodule update --force --remote --recursive")

add_subdirectory(JTOK)
add_subdirectory(drivers)

if(WIN32)
    message("GOT HERE")
    if(CMAKE_CROSSCOMPILING STREQUAL "OFF")
        
        add_subdirectory(termiWin)
    endif()
endif()
add_subdirectory(application)


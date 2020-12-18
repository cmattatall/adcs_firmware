################################################################################
# @brief Top level CMake file for the ADCS project
# @author: Carl Mattatall (cmattatall2@gmail.com)
#
################################################################################
# USEFUL DOCUMENTATION SECTION FOR THOSE NEW TO CMAKE
################################################################################
#
# best practices:
#   https://cliutils.gitlab.io/modern-cmake/
#
# variables:
#   https://cmake.org/cmake/help/latest/manual/cmake-variables.7.html
#
# booleans:
#   https://stackoverflow.com/questions/41079291/cmake-use-of-true-false-or-on-off-or-yes-no-in-generator-expressions
#
# behaviour of >if< 
#   https://cmake.org/cmake/help/v3.18/command/if.html
#
################################################################################

set(SUPPORTED_BUILD_TYPES "")
list(APPEND SUPPORTED_BUILD_TYPES "Debug")
list(APPEND SUPPORTED_BUILD_TYPES "Release")


cmake_minimum_required(VERSION 3.16)

set(CMAKE_CROSSCOMPILING OFF BOOL CACHE STRING "[ON/OFF] Boolean to choose to cross compile or not")

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type chosen by the user at configure time")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${SUPPORTED_BUILD_TYPES})

set(MSP430_MCU "msp430f5529" CACHE STRING "Lowercase MPN of target MCU. Used by msp430-elf-gcc")
set(MSP430_MCU_FREQ "8000000" CACHE STRING "Frequency of input oscillator to MCU in Hz")

################################################################################
#                       PROJECT START                                          #
################################################################################
project(
    ADCS_application 
    VERSION 0.1
    DESCRIPTION "ADCS Firmware Project for DSS LORIS Satellite"
    LANGUAGES C ASM
)

message("\nPROJECT : ${CMAKE_PROJECT_NAME}")
message("BUILD_TARGET : ${CMAKE_SYSTEM_NAME}")
message("PROCESSOR : ${CMAKE_SYSTEM_PROCESSOR}")

if(NOT ${CMAKE_BUILD_TYPE} IN_LIST SUPPORTED_BUILD_TYPES)
    message("\n")
    message("\"${CMAKE_BUILD_TYPE}\" IS NOT A SUPPORTED BUILD TYPE.\nAccepted values:")
    foreach(type ${SUPPORTED_BUILD_TYPES})
        message("- \"${type}\"")
    endforeach(type ${SUPPORTED_BUILD_TYPES})
    message("\n")
    return()
else()
    message("CMAKE_BUILD_TYPE : \"${CMAKE_BUILD_TYPE}\"")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options("-Wall")
    add_compile_options("-Wshadow")
    add_compile_options("-g")
    add_compile_options("-ggdb")
else()
    add_compile_options("-O3")
endif()

function(ADCS_add_executable target)
    if(CMAKE_CROSSCOMPILING)
        msp430_add_executable(${target} ${ARGN})
    else()  # NOT CROSS COMPILING
        add_executable(${target} ${ARGN})
    endif(CMAKE_CROSSCOMPILING)
endfunction(ADCS_add_executable target)

function(ADCS_add_library target)
    if(CMAKE_CROSSCOMPILING)
        msp430_add_library(${target} ${ARGN})
    else()  # NOT CROSS COMPILING
        add_library(${target} ${ARGN})
    endif(CMAKE_CROSSCOMPILING)
endfunction(ADCS_add_library target)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/shared/inc")

# for some reason, you have to manually run this command. I haven't got it working yet
# FOR THE FIRST TIME, DO 
# $git submodule update --force --remote --recursive --init
# NOTICE THE --init on the end.
execute_process(
    COMMAND "git" "submodule" "update" "--remote" "--recursive"
    OUTPUT_VARIABLE GIT_SUBMODULE_UPDATE_OUTPUT_VARIABLE
    RESULT_VARIABLE GIT_SUBMODULE_UPDATE_RESULT_VARIABLE
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ECHO_OUTPUT_VARIABLE
    )

add_subdirectory(JTOK)
add_subdirectory(drivers)
add_subdirectory(application)




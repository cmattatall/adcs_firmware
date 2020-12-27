cmake_minimum_required(VERSION 3.16)
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
#
# OTHER : 
#   on windows, you probably need to install gcc
#   https://sourceforge.net/projects/mingw-w64/
#   
#
#
################################################################################
option(BUILD_TESTING "[ON/OFF] Boolean to choose to cross compile or not" OFF)
option(CMAKE_CROSSCOMPILING "[ON/OFF] Boolean to choose to cross compile or not" OFF)


set(SUPPORTED_BUILD_TYPES "")
list(APPEND SUPPORTED_BUILD_TYPES "Debug")
list(APPEND SUPPORTED_BUILD_TYPES "Release")

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type chosen by the user at configure time")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${SUPPORTED_BUILD_TYPES})

set(MSP430_MCU "msp430f5529" CACHE STRING "Lowercase MPN of target MCU. Used by msp430-elf-gcc")
set(MSP430_MCU_FREQ "8000000" CACHE STRING "Frequency of input oscillator to MCU in Hz")

################################################################################
#                       PROJECT START                                          #
################################################################################

get_filename_component(MY_TOPLEVEL_PROJECT_NAME ${CMAKE_SOURCE_DIR} NAME)

project(
    ${MY_TOPLEVEL_PROJECT_NAME}
    VERSION 0.1
    DESCRIPTION "ADCS Firmware Project for DSS LORIS Satellite"
    LANGUAGES C ASM
)

# I'd like to configure this in the toolchain file, BUT, we don't actually
# have access to project variables when cmake processes the toolchain file
# since the top-level "project" instruction is actually what triggers
# toolchain configuration.
# Thus, we have to set this after we process the toolchain, but before
# we add any executable/library targets to the project
#
# if cmake supported CMAKE_PROJECT_BINARY_DIR instead of just
#
# PROJECT_BINARY_DIR , it would be SOOOOO useful :(
#
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} CACHE INTERNAL "")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} CACHE INTERNAL "")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} CACHE INTERNAL "")

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


function(ADCS_add_executable target)
    if(CMAKE_CROSSCOMPILING)
        msp430_add_executable(${target} ${ARGN})
    else()  # NOT CROSS COMPILING
        add_executable(${target} ${ARGN})
    endif(CMAKE_CROSSCOMPILING)
endfunction(ADCS_add_executable target)

function(ADCS_add_library target)
    if(CMAKE_CROSSCOMPILING)
        msp430_add_library(${target} STATIC ${ARGN})
    else()  # NOT CROSS COMPILING
        add_library(${target} STATIC ${ARGN})
    endif(CMAKE_CROSSCOMPILING)
endfunction(ADCS_add_library target)

message("\n")
execute_process(
    COMMAND "git" "submodule" "update" "--remote" "--recursive" "--force"
    RESULT_VARIABLE GIT_SUBMODULE_UPDATE_FAILED
    OUTPUT_VARIABLE GIT_SUBMODULE_UPDATE_OUTPUT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    #ECHO_OUTPUT_VARIABLE
    )

message("${GIT_SUBMODULE_UPDATE_OUTPUT}")

execute_process(
    COMMAND "sync"
)


if(GIT_SUBMODULE_UPDATE_FAILED)
    message(FATAL_ERROR "Could not update some or all submodules. Aborting build!")
endif(GIT_SUBMODULE_UPDATE_FAILED)

if(BUILD_TESTING)
    include(CTest)
endif()

include_directories(share)

add_subdirectory(share)
add_subdirectory(JTOK)
add_subdirectory(bufferlib)
add_subdirectory(drivers)
add_subdirectory(core)
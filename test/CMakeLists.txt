cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "CMAKE_CROSSCOMPILING IS NOT DEFINED. Please define it as \"ON\" or \"OFF\"")
    return()
endif()


# TOOLCHAIN MAGIC GETS ACTIVATED UPON SEEING THE project COMMAND
project(test_project 
    VERSION 0.1
    DESCRIPTION "ADCS Firmware Project for DSS LORIS Satellite"
    LANGUAGES C ASM)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


list(APPEND SUPPORTED_BUILD_TYPES "Debug")
list(APPEND SUPPORTED_BUILD_TYPES "Release")
list(APPEND SUPPORTED_BUILD_TYPES "RelWithDebInfo")
list(APPEND SUPPORTED_BUILD_TYPES "MinSizeRel")
if(NOT ${CMAKE_BUILD_TYPE} IN_LIST SUPPORTED_BUILD_TYPES)
    message(FATAL_ERROR "${CMAKE_BUILD_TYPE} is not a supported build type")
    message("Supported build types:")
    foreach(btype ${SUPPORTED_BUILD_TYPES})
        message("- \"${btype}\"")    
    endforeach(btype ${SUPPORTED_BUILD_TYPES})
endif(NOT ${CMAKE_BUILD_TYPE} IN_LIST SUPPORTED_BUILD_TYPES)









add_executable(blah)

target_sources(blah PRIVATE main.c)
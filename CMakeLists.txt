cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "CMAKE_CROSSCOMPILING IS NOT DEFINED. Please define it as \"ON\" or \"OFF\"")
    return()
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

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

set(MCU_MPN "msp430f5529") #set this as a lowercase string of your part number
set(MCU_FREQ "8000000")    #set this as the HSE input freq in Hz

################################################################################
#                       PROJECT START                                          #
################################################################################
project(
    ADCS_application 
    VERSION 0.1
    DESCRIPTION "ADCS Firmware Project for DSS LORIS Satellite"
    LANGUAGES C ASM
)

message("") 
message("PROJECT : ${CMAKE_PROJECT_NAME}")
message("TARGET : ${CMAKE_SYSTEM_NAME}")
message("PROCESSOR : ${CMAKE_SYSTEM_PROCESSOR}")
message("") 

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#    add_compile_options("-Wall")
#    add_compile_options("-Wshadow")
#    add_compile_options("-g")
#    add_compile_options("-ggdb")
#endif()

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


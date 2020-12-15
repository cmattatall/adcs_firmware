cmake_minimum_required(VERSION 3.16)

###################################################################
##              USER CONFIGURED 
###################################################################
# - when cross compiling is off, we build to run on our host system
# 
# - when cross compiling is on, we build for target device
###################################################################
set(CMAKE_CROSSCOMPILING OFF)
project(adcs_firmware LANGUAGES C ASM)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options("-Wall")
    add_compile_options("-Wshadow")
endif()

# for some reason, you have to manually run this command. I haven't got it working yet
# FOR THE FIRST TIME, DO 
# $git submodule update --force --remote --recursive --init
# NOTICE THE --init on the end.
execute_process(COMMAND "git submodule update --force --remote --recursive")
add_subdirectory(JTOK)
set(BUILD_TARGET ${CMAKE_PROJECT_NAME})

if(CMAKE_CROSSCOMPILING STREQUAL "ON")
    set(MSP430_MCU "msp430f5529")
    set(MSP430_MCU_FREQ "8000000")
    msp430_add_executable(${BUILD_TARGET})
else()
    add_executable(${BUILD_TARGET})
    find_package(Threads REQUIRED)
    if(CMAKE_USE_PTHREADS_INIT)
        target_link_libraries(${BUILD_TARGET} PUBLIC pthread)
        target_compile_options(${BUILD_TARGET} PUBLIC "-lpthread")
    else()
        message(FATAL_ERROR "pthread is the only supported library for native platform firmware test")
    endif()
endif(CMAKE_CROSSCOMPILING STREQUAL "ON")

#headers and sources
file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/application/src/*.c")
file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/application/inc/*.h")
foreach(hdr ${headers})
    get_filename_component(hdr_dir ${hdr} DIRECTORY)
    list(APPEND include_dirs ${hdr_dir})
endforeach(hdr ${headers})
list(REMOVE_DUPLICATES include_dirs)

#link
target_sources(${BUILD_TARGET} PRIVATE "${sources}")
target_include_directories(${BUILD_TARGET} PUBLIC "${include_dirs}")
target_link_libraries(${BUILD_TARGET} PUBLIC JTOK)


add_subdirectory(drivers)
target_link_libraries(${BUILD_TARGET} PUBLIC MSP430F5529_DRIVERS)

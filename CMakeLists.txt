cmake_minimum_required(VERSION 3.16)

project(adcs_firmware LANGUAGES C ASM)

set(BUILD_TARGET ${CMAKE_PROJECT_NAME})

set(MSP430_MCU "msp430f5529")
set(MSP430_MCU_FREQ "8000000")

msp430_add_executable(${BUILD_TARGET})

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
target_sources(${BUILD_TARGET} PRIVATE "${sources}")

file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h")
foreach(hdr ${headers})
    get_filename_component(hdr_dir ${hdr} DIRECTORY)
    list(APPEND include_dirs ${hdr_dir})
endforeach(hdr ${headers})
list(REMOVE_DUPLICATES include_dirs)
target_include_directories(${BUILD_TARGET} PUBLIC "${include_dirs}")
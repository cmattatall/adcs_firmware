cmake_minimum_required(VERSION 3.16)
project(adcs_firmware LANGUAGES C ASM)

if(NOT EXISTS JTOK)
    execute_process(COMMAND "git clone https://github.com/cmattatall/JTOK.git")
endif()
add_subdirectory(JTOK)

set(BUILD_TARGET ${CMAKE_PROJECT_NAME})

#target defs
set(MSP430_MCU "msp430f5529")
set(MSP430_MCU_FREQ "8000000")

msp430_add_executable(${BUILD_TARGET})

#headers and sources
file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h")
foreach(hdr ${headers})
    get_filename_component(hdr_dir ${hdr} DIRECTORY)
    list(APPEND include_dirs ${hdr_dir})
endforeach(hdr ${headers})
list(REMOVE_DUPLICATES include_dirs)

#link
target_sources(${BUILD_TARGET} PRIVATE "${sources}")
target_include_directories(${BUILD_TARGET} PUBLIC "${include_dirs}")
target_link_libraries(${BUILD_TARGET} PUBLIC JTOK)



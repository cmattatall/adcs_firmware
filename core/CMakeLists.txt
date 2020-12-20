cmake_minimum_required(VERSION 3.16)
project(
    adcs 
    VERSION 0.1
    DESCRIPTION "Core firmware components (HW independent) for ADCS project "
    LANGUAGES C ASM
)

if(NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(emulated)
endif(NOT CMAKE_CROSSCOMPILING)

add_subdirectory(jsons)
add_subdirectory(magnetorquers)
add_subdirectory(reaction_wheels)


# OBC INTERFACE
ADCS_add_library(ADCS_OBC_INTERFACE)
target_sources(ADCS_OBC_INTERFACE PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/obc_interface/obc_interface.c")
target_include_directories(ADCS_OBC_INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/obc_interface")
target_link_libraries(ADCS_OBC_INTERFACE PUBLIC INJECTION_API)
target_link_libraries(ADCS_OBC_INTERFACE PUBLIC BUFFERLIB)


# FIRMWARE EXECUTABLE 
# (just main.c, we link against everything else based on target device)
ADCS_add_executable(ADCS_FIRMWARE_EXECUTABLE)
target_sources(ADCS_FIRMWARE_EXECUTABLE PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/main.c")
target_link_libraries(ADCS_FIRMWARE_EXECUTABLE PUBLIC ADCS_JSONS)
target_link_libraries(ADCS_FIRMWARE_EXECUTABLE PUBLIC ADCS_OBC_INTERFACE)
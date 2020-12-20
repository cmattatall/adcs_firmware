cmake_minimum_required(VERSION 3.16)

option(BUILD_TESTING "[ON/OFF] Boolean to choose to cross compile or not" OFF)

if(BUILD_TESTING)
    message("WE GONNA BUILD THE TESTS FOR CORE")
endif()

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
add_subdirectory(obc_interface)

# FIRMWARE EXECUTABLE 
ADCS_add_executable(ADCS_FIRMWARE_EXECUTABLE)
target_sources(ADCS_FIRMWARE_EXECUTABLE PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/main.c")
target_link_libraries(ADCS_FIRMWARE_EXECUTABLE PUBLIC ADCS_JSONS)
target_link_libraries(ADCS_FIRMWARE_EXECUTABLE PUBLIC ADCS_OBC_INTERFACE)
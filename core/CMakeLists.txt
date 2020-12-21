cmake_minimum_required(VERSION 3.16)

option(BUILD_TESTING "[ON/OFF] Boolean to choose to cross compile or not" OFF)

if(BUILD_TESTING)
    include(CTest)
endif(BUILD_TESTING)

project(
    adcs 
    VERSION 0.1
    DESCRIPTION "Core firmware components (HW independent) for ADCS project "
    LANGUAGES C ASM
)

set(EXE "${PROJECT_NAME}")

if(NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(emulated)
endif(NOT CMAKE_CROSSCOMPILING)

add_subdirectory(jsons)
add_subdirectory(magnetorquers)
add_subdirectory(reaction_wheels)
add_subdirectory(obc_interface)

# FIRMWARE EXECUTABLE 
ADCS_add_executable(${EXE})
target_sources(${EXE} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/main.c")
target_link_libraries(${EXE} PUBLIC ADCS_JSONS)
target_link_libraries(${EXE} PUBLIC ADCS_OBC_INTERFACE)
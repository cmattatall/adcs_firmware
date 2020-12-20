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

# MAGNETORQUERS
ADCS_add_library(ADCS_MAGNETORQUERS)
target_sources(ADCS_MAGNETORQUERS PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/magnetorquers.c")
target_include_directories(ADCS_MAGNETORQUERS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
if(CMAKE_CROSSCOMPILING)
    target_link_libraries(ADCS_MAGNETORQUERS PUBLIC ADCS_DRIVERS)
else()
    target_link_libraries(ADCS_MAGNETORQUERS PUBLIC ADCS_IF_EMU)
endif(CMAKE_CROSSCOMPILING)


# REACTION WHEELS
ADCS_add_library(ADCS_REACTIONWHEELS)
target_sources(ADCS_REACTIONWHEELS PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/reaction_wheels.c")
target_include_directories(ADCS_REACTIONWHEELS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
if(CMAKE_CROSSCOMPILING)
    target_link_libraries(ADCS_REACTIONWHEELS PUBLIC ADCS_DRIVERS)
else()
    target_link_libraries(ADCS_REACTIONWHEELS PUBLIC ADCS_IF_EMU)
endif(CMAKE_CROSSCOMPILING)


# JSONS
ADCS_add_library(ADCS_JSONS)
target_sources(ADCS_JSONS PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/jsons/jsons.c")
target_include_directories(ADCS_JSONS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/jsons")
target_link_libraries(ADCS_JSONS PUBLIC JTOK)
target_link_libraries(ADCS_JSONS PUBLIC ADCS_OBC_INTERFACE)
target_link_libraries(ADCS_JSONS PUBLIC ADCS_REACTIONWHEELS)
target_link_libraries(ADCS_JSONS PUBLIC ADCS_MAGNETORQUERS)

# OBC INTERFACE
ADCS_add_library(ADCS_OBC_INTERFACE)
target_sources(ADCS_OBC_INTERFACE PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/obc_interface/obc_interface.c")
target_include_directories(ADCS_OBC_INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/obc_interface")
target_link_libraries(ADCS_OBC_INTERFACE PUBLIC INJECTION_API)
target_link_libraries(ADCS_OBC_INTERFACE PUBLIC BUFFERLIB)


# FIRMWARE EXECUTABLE (literally only main.c, we link against everything else)
ADCS_add_executable(ADCS_FIRMWARE_EXECUTABLE)
target_sources(ADCS_FIRMWARE_EXECUTABLE PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")
target_include_directories(ADCS_FIRMWARE_EXECUTABLE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_libraries(ADCS_FIRMWARE_EXECUTABLE PUBLIC ADCS_JSONS)
target_link_libraries(ADCS_FIRMWARE_EXECUTABLE PUBLIC ADCS_OBC_INTERFACE)
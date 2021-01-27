cmake_minimum_required(VERSION 3.16)

option(BUILD_TESTING "[ON/OFF] Boolean to choose to cross compile or not" OFF)

################################################################################
#  BUILD TYPE CHECK
################################################################################
if(NOT CMAKE_PROJECT_NAME)
    set(SUPPORTED_BUILD_TYPES "")
    list(APPEND SUPPORTED_BUILD_TYPES "Debug")
    list(APPEND SUPPORTED_BUILD_TYPES "Release")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${SUPPORTED_BUILD_TYPES})
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type chosen by the user at configure time")
    else()
        if(NOT CMAKE_BUILD_TYPE IN_LIST SUPPORTED_BUILD_TYPES)
            message("Build type : ${CMAKE_BUILD_TYPE} is not a supported build type.")
            message("Supported build types are:")
            foreach(type ${SUPPORTED_BUILD_TYPES})
                message("- ${type}")
            endforeach(type ${SUPPORTED_BUILD_TYPES})
            message(FATAL_ERROR "The configuration script will now exit.")
        endif(NOT CMAKE_BUILD_TYPE IN_LIST SUPPORTED_BUILD_TYPES)
    endif(NOT CMAKE_BUILD_TYPE)
endif(NOT CMAKE_PROJECT_NAME)

project(
    adcs 
    VERSION 0.1
    DESCRIPTION "Core firmware components (HW independent) for ADCS project "
    LANGUAGES C ASM
)
set(EXE "${PROJECT_NAME}_executable")
message("Configuring target : ${PROJECT_NAME}")

add_subdirectory(jsons)
add_subdirectory(magnetorquers)
add_subdirectory(reaction_wheels)
add_subdirectory(obc_interface)
add_subdirectory(sun_sensors)
add_subdirectory(accelerometer)
add_subdirectory(inertial_measurement_unit)

if(NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(emulated)
endif(NOT CMAKE_CROSSCOMPILING)

# FIRMWARE EXECUTABLE 
add_executable(${EXE})
target_sources(${EXE} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/main.c")
target_link_libraries(${EXE} PRIVATE ADCS_JSONS)
target_link_libraries(${EXE} PRIVATE ADCS_OBC_INTERFACE)
target_link_libraries(${EXE} PRIVATE ADCS_SUN_SENSORS)
target_link_libraries(${EXE} PRIVATE ADCS_ACCELEROMETER)
target_link_libraries(${EXE} PRIVATE ADCS_IMU)

if(NOT CMAKE_CROSSCOMPILING)
    target_link_libraries(${EXE} PRIVATE ADCS_IF_EMU)
else()
    target_link_libraries(${EXE} PRIVATE ADCS_DRIVERS)
endif(NOT CMAKE_CROSSCOMPILING)

################################################################################
# TEST CONFIGURATION
################################################################################
if(BUILD_TESTING)
    enable_testing()
    include(CTest)
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)
        add_subdirectory(test)
    endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)
else()
    if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
        add_compile_options("-Wall")
        add_compile_options("-Wextra")
        enable_testing()
        include(CTest)
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)
            add_subdirectory(test)
        endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)
    endif()
endif()
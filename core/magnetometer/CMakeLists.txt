cmake_minimum_required(VERSION 3.18)


################################################################################
#  OPTIONS GO HERE
################################################################################
option(BUILD_TESTING "[ON/OFF] Build tests in addition to library" OFF)
option(BUILD_EXAMPLES "[ON/OFF] Build examlples in addition to library" ON)


################################################################################
#  PROJECT INIT
################################################################################
project(
    ADCS_MAGNETOMETER
    VERSION 1.0
    DESCRIPTION "MAGNETOMETER INTERFACE LIBRARY FOR ADCS FIRMWARE"
    LANGUAGES C CXX
)


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


################################################################################
# DETECT SOURCES RECURSIVELY FROM src FOLDER AND ADD TO BUILD TARGET
################################################################################
set(LIB "${PROJECT_NAME}") # this is PROJECT_NAME, NOT CMAKE_PROJECT_NAME
message("CONFIGURING TARGET : ${LIB}")

if(TARGET ${LIB})
    message(FATAL_ERROR "Target ${LIB} already exists in this project!")
else()
    add_library(${LIB})
endif(TARGET ${LIB})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
file(GLOB_RECURSE ${LIB}_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
target_sources(${LIB} PRIVATE ${${LIB}_sources})


################################################################################
# DETECT PRIVATE HEADERS RECURSIVELY FROM src FOLDER
################################################################################
file(GLOB_RECURSE ${LIB}_private_headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
set(${LIB}_private_include_directories "")
foreach(hdr ${${LIB}_private_headers})
    get_filename_component(hdr_dir ${hdr} DIRECTORY)
    list(APPEND ${LIB}_private_include_directories ${hdr_dir})
endforeach(hdr ${${LIB}_private_headers})
list(REMOVE_DUPLICATES ${LIB}_private_include_directories)
target_include_directories(${LIB} PRIVATE ${${LIB}_private_include_directories})


################################################################################
# DETECT PUBLIC HEADERS RECURSIVELY FROM inc FOLDER
################################################################################
file(GLOB_RECURSE ${LIB}_public_headers "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h")
set(${LIB}_public_include_directories "")
foreach(hdr ${${LIB}_public_headers})
    get_filename_component(hdr_dir ${hdr} DIRECTORY)
    list(APPEND ${LIB}_public_include_directories ${hdr_dir})
endforeach(hdr ${${LIB}_public_headers})
list(REMOVE_DUPLICATES ${LIB}_public_include_directories)
target_include_directories(${LIB} PUBLIC ${${LIB}_public_include_directories})


################################################################################
# SPECIAL AND PROJECT SPECIFIC OPTIONS
################################################################################
target_compile_options(${LIB} PRIVATE "-Werror=incompatible-pointer-types")
target_compile_options(${LIB} PRIVATE "-Wshadow")






################################################################################
# LINK AGAINST THE NECESSARY LIBRARIES 
################################################################################
if(NOT CMAKE_CROSSCOMPILING)
    target_link_libraries(${LIB} PRIVATE ADCS_IF_EMU)
else()
    target_link_libraries(${LIB} PRIVATE ADCS_DRIVERS)
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


################################################################################
# EXAMPLE CONFIGURATION
################################################################################
if(BUILD_EXAMPLES)
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples)
        add_subdirectory(examples)
    endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples)
else()
    if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
        add_compile_options("-Wall")
        add_compile_options("-Wextra")
        enable_testing()
        include(CTest)
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples)
            add_subdirectory(examples)
        endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples)
    endif()
endif(BUILD_EXAMPLES)


















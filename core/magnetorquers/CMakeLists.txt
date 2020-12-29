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
    ADCS_MAGNETORQUERS 
    VERSION 1.0 
    DESCRIPTION "(OPTIONALLY) THREAD-SAFE BUFFER API LIBRARY"
    LANGUAGES C
)
set(LIB "${PROJECT_NAME}")
message("CONFIGURING TARGET : ${LIB}")
ADCS_add_library("${LIB}")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # vscode

file(GLOB_RECURSE "${LIB}_sources" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
target_sources(${LIB} PRIVATE "${${LIB}_sources}")

file(GLOB_RECURSE "${LIB}_headers" "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h")

set("${LIB}_include_directories" "")
foreach(hdr ${${LIB}_headers})
    get_filename_component(dir "${hdr}" DIRECTORY)
    list(APPEND "${LIB}_include_directories" ${dir})
endforeach(hdr ${${LIB}_headers})

list(REMOVE_DUPLICATES ${LIB}_include_directories)
target_include_directories(${LIB} PUBLIC ${${LIB}_include_directories})

if(CMAKE_CROSSCOMPILING)
    target_link_libraries(${LIB} PUBLIC ADCS_DRIVERS)
else()
    target_link_libraries(${LIB} PUBLIC ADCS_IF_EMU)
endif(CMAKE_CROSSCOMPILING)

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

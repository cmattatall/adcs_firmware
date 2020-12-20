cmake_minimum_required(VERSION 3.18)
project(
    BUFFERLIB 
    VERSION 1.0 
    DESCRIPTION "STATICALLY ALLOCATED JSON LIBRARY FOR EMBEDDED SYSTEMS. LOOSELY BASED ON JSMN"
    LANGUAGES C
)
set(BUILD_TARGET ${PROJECT_NAME})
add_library(${BUILD_TARGET})

message("BUILD_TARGET = ${BUILD_TARGET}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # vscode

file(GLOB_RECURSE sources 
"${CMAKE_CURRENT_SOURCE_DIR}/src/*.c" 
)

file(GLOB_RECURSE internal_headers 
"${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h" 
)

foreach(hdr ${internal_headers})
    get_filename_component(hdr_dir ${hdr} DIRECTORY)
    list(APPEND internal_include_dirs ${hdr_dir})
    endforeach(hdr ${internal_headers})
list(REMOVE_DUPLICATES internal_include_dirs)

message("internal_include_dirs = ${internal_include_dirs}")


target_include_directories(${BUILD_TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${BUILD_TARGET} PRIVATE ${internal_include_dirs})
target_sources(${BUILD_TARGET} PRIVATE ${sources})

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
    option(PACKAGE_TESTS "Build the tests" ON)
endif()

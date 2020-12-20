cmake_minimum_required(VERSION 3.18)

option(BUILD_TESTING "[ON/OFF] Boolean to choose to cross compile or not" OFF)

project(
    ADCS_REACTIONWHEELS 
    VERSION 1.0 
    DESCRIPTION "(OPTIONALLY) THREAD-SAFE BUFFER API LIBRARY"
    LANGUAGES C
)
set(LIB "${PROJECT_NAME}")
message("CONFIGURING TARGET : ${LIB}")
add_library(${LIB})
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

if(BUILD_TESTING)
    include(CTest)
    add_subdirectory(test)
else()
    # If we're working on this project 
    # we should be doing all the tests anyway
    if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
        include(CTest)
        add_subdirectory(test)
    endif()
endif()
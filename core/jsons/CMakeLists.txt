cmake_minimum_required(VERSION 3.18)

option(BUILD_TESTING "[ON/OFF] Boolean to choose to cross compile or not" OFF)

project(
    ADCS_JSONS 
    VERSION 1.0 
    DESCRIPTION "ADCS <-> OBC JSON COMMUNICATION LIBRARY FOR LORRIS PROJECT"
    LANGUAGES C ASM
)
set(LIB "${PROJECT_NAME}")
message("CONFIGURING TARGET : ${LIB}")
add_library(${LIB})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # vscode

file(GLOB_RECURSE ${LIB}_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
target_sources(${LIB} PRIVATE ${${LIB}_sources})

file(GLOB_RECURSE ${LIB}_headers "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h")
set(${LIB}_public_include_dirs "")
foreach(hdr ${${LIB}_headers})
    get_filename_component(dir ${hdr} DIRECTORY)
    list(APPEND ${LIB}_public_include_dirs ${dir})
endforeach(hdr ${${LIB}_headers})
list(REMOVE_DUPLICATES ${LIB}_public_include_dirs)
target_include_directories(${LIB} PUBLIC ${${LIB}_public_include_dirs})

# Link against the necessary things
target_link_libraries(${LIB} PUBLIC JTOK)
target_link_libraries(${LIB} PUBLIC ADCS_OBC_INTERFACE)
target_link_libraries(${LIB} PUBLIC ADCS_REACTIONWHEELS)
target_link_libraries(${LIB} PUBLIC ADCS_MAGNETORQUERS)

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

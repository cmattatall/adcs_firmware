cmake_minimum_required(VERSION 3.18)

project(
    ADCS_JSONS 
    VERSION 1.0 
    DESCRIPTION "ADCS <-> OBC JSON COMMUNICATION LIBRARY FOR LORRIS PROJECT"
    LANGUAGES C ASM
)
message("CONFIGURING TARGET : ${PROJECT_NAME}")
set(LIB ${PROJECT_NAME})
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

# this is a guard so we don't run tests on LITERALLY every library we import
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    
    # GENERATE DEBUG SYMBOLS IF WE'RE GOING TO TEST STUFF
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Debug")
        target_compile_options(${LIB} PRIVATE "-Wall")
    endif(NOT CMAKE_BUILD_TYPE)

    include(CTest)
    enable_testing()
    add_subdirectory(test)
    option(PACKAGE_TESTS "Build the tests" ON)
endif()

################################################################################ 
# 
# @brief CMAKE TEST SCRIPT FOR ADCS DRIVERS
# @author Carl Mattatall (cmattatall2@gmail.com)
#
################################################################################
cmake_minimum_required(VERSION 3.16)

if(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(BACKUP_CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif(CMAKE_RUNTIME_OUTPUT_DIRECTORY)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

if(CMAKE_CROSSCOMPILING)
    file(GLOB test_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
    foreach(src ${test_sources})
        get_filename_component(test_suffix ${src} NAME_WLE)
        set(test_target "${LIB}_${test_suffix}")
        if(NOT TARGET ${test_target})
            add_executable(${test_target})
            target_sources(${test_target} PRIVATE ${src})
            
            if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
                target_compile_options(${test_target} PRIVATE "-Wall")
                target_compile_options(${test_target} PRIVATE "-Wshadow")
            endif(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

            target_link_libraries(${test_target} PRIVATE ${LIB})
            add_test(
                NAME ${test_target}
                COMMAND valgrind ${CMAKE_CURRENT_BINARY_DIR}/${test_target}
                --build-generator "${CMAKE_GENERATOR}"
                --test-command "${CMAKE_CTEST_COMMAND}"
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            ) 
        endif(NOT TARGET ${test_target})
        unset(${LIB}_TEST_DIR)
        unset(test_target)
    endforeach(src ${test_sources})
else()
    message("Peripheral driver tests will not be built because we are not cross compiling")
endif(CMAKE_CROSSCOMPILING)

if(BACKUP_CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BACKUP_CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif(BACKUP_CMAKE_RUNTIME_OUTPUT_DIRECTORY)

################################################################################ 
# 
# @brief CMAKE TEST SCRIPT FOR ADCS JSONS ON ADCS PROJECT
# @author Carl Mattatall (cmattatall2@gmail.com)
#
################################################################################ 

cmake_minimum_required(VERSION 3.16)
if(CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "Native tests cannot be performed on the target MCU")
else()

    # this is a guard so we don't run tests on LITERALLY every library we import
    if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
        add_executable(valid_json_test)
        target_sources(valid_json_test PRIVATE "valid_json_test.c")
        target_link_libraries(valid_json_test PUBLIC ADCS_JSONS)
        add_test(
            valid_json_test_TEST valid_json_test
            COMMAND valid_json_test
            --build-generator "${CMAKE_GENERATOR}"
            --test-command "${CMAKE_CTEST_COMMAND}"
        )

        add_executable(invalid_json_test)
        target_sources(invalid_json_test PRIVATE "invalid_json_test.c")
        target_link_libraries(valid_json_test PUBLIC ADCS_JSONS)
        add_test(
            invalid_json_test_TEST invalid_json_test
            COMMAND invalid_json_test
            --build-generator "${CMAKE_GENERATOR}"
            --test-command "${CMAKE_CTEST_COMMAND}"
        )

    endif(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
endif(CMAKE_CROSSCOMPILING)





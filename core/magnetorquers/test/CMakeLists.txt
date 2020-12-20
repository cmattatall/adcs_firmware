################################################################################ 
# 
# @brief CMAKE TEST SCRIPT FOR MAGNETORQUERS
# @author Carl Mattatall (cmattatall2@gmail.com)
#
################################################################################ 

cmake_minimum_required(VERSION 3.16)
if(CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "Native tests cannot be performed on the target MCU")
else()

    # this is a guard so we don't run tests on LITERALLY every library we import
    if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    # @todo : ADD TESTS

    endif(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
endif(CMAKE_CROSSCOMPILING)
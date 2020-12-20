################################################################################ 
# @brief
# CMAKE TEST SCRIPT FOR HARDWARD SPECIFIC
# ON ADCS PROJECT
# 
# @author Carl Mattatall (cmattatall2@gmail.com)
#
#
################################################################################
cmake_minimum_required(VERSION 3.16)

if(CMAKE_CROSSCOMPILING)
    if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

        # @todo ADD THE TEST STUFF HERE

    endif()
else()
    message(FATAL_ERROR "HARDWARE SPECIFIC TESTS MUST BE RUN ON THE MCU")
endif(CMAKE_CROSSCOMPILING)
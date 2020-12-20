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

    ## TODO, ADD THE TESTS

    endif(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
endif(CMAKE_CROSSCOMPILING)





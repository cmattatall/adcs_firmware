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

    # @todo ADD TESTS

endif(CMAKE_CROSSCOMPILING)